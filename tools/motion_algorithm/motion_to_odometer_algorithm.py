from math import atan2, sqrt, pi
import logging
import argparse
from pathlib import Path
import json


from utils import get_wheels_base, get_wheel_radius, get_max_n


logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[logging.StreamHandler()]
)

logger = logging.getLogger(__name__)


class MotionToOdometer:
    def __init__(self, wheel_radius, max_n, wheels_base):
        self._wheel_radius = wheel_radius
        self._max_n = max_n
        self._wheels_base = wheels_base
        logger.info(
            f'Wheel radius: "{self._wheel_radius}" '
            f'max N: "{self._max_n}" '
            f'wheels base: "{self._wheels_base}"')

    def __call__(self, x1, y1, phi1, x2, y2,):
        # x1, y1 - coordinates of the initial point
        # phi1 - angle of the initial point
        # x2, y2 - coordinated of the next point

        phi2 = atan2(y2 - y1, x2 - x1)
        dist_m = sqrt((x2 - x1)**2 + (y2 - y1)**2)
        delta_phi = phi2 - phi1

        # step 1 - change phi angle
        angle_n_l, angle_n_r = self._convert_delta_phi_to_n(
            delta_phi)

        # step 2 - change dist
        dist_n = self._convert_dist_m_to_n(
            dist_m)

        return angle_n_l, angle_n_r, dist_n, phi2


    # Motion along the line
    def _calc_delta_n_for_dist(self, delta_d):
        # R - wheel radius
        # max_n - count of holes in odometer
        # 2 * pi * R ~ max_n
        # delta_d - n
        n = (delta_d * self._max_n) / (2 * pi * self._wheel_radius) 
        return n

    # a question - how many n's should be to rotate on the angle phi?
    # input: phi
    # output: n (or -n)

    # parameters:
    # wheels_base - dist in m between wheels
    # wheel_radius - radius of the wheel in meters?

    # step 1
    # convert phi into the distance (meters)
    # 2 * pi * (wheels_base / 2.0) ~ 2 * pi
    # x (a dist in meters) ~ phi
    # x =  phi * (wheels_base / 2.0) <- dist m

    def _convert_delta_phi_to_dist_m(self, delta_phi):
        dist_m = abs(delta_phi) * (self._wheels_base / 2.0)
        logger.info(f'For delta phi: {delta_phi:.3f} dist: {dist_m:.3f}')
        if delta_phi < 0:
            dist_l_m = dist_m
            dist_r_m = -dist_m
        else:
            dist_l_m = -dist_m
            dist_r_m = dist_m
        return dist_l_m, dist_r_m

    # step 2
    # convert dist in meters to n (count of holes for the odometer)
    # 2 * pi * wheel_radius ~ max_n
    # dist ~ n
    # n = (dist * max_n) / (2 * pi * wheel_radius)

    def _convert_dist_m_to_n(self, dist_m):
        n = (dist_m * self._max_n) / (2 * pi * self._wheel_radius)
        logger.info(f'For dist_m: {dist_m:.3f} n: {n:.3f}')
        n = round(n)
        logger.info(f'Rounding n: {n}')
        return n


    def _convert_delta_phi_to_n(self, delta_phi):
        dist_l_m, dist_r_m = self._convert_delta_phi_to_dist_m(delta_phi)
        n_l = self._convert_dist_m_to_n(dist_l_m)
        # we can call a function second time.
        # but it's obvious that the only difference is opposite sign
        n_r = - n_l
        return n_l, n_r


def main():
    logger.info('Parsing arguments')
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--config', 
        required=True,
        type=Path,
        help='Path to configuration file')
    parser.add_argument(
        '--motion_file',
        required=True,
        type=Path,
        help="Path to file with path values. "
        "Example of the format: {'x': [], 'y': []}")
    parser.add_argument(
        '--res_file',
        required=True,
        type=Path,
        help='Path to the result file with odometer values '
        "Example of the format: {'n_left': [], 'n_right': []}")
    args = parser.parse_args()
    logger.info(f'Arguments are: {args}')
    motion_file_path = args.motion_file
    res_file_path = args.res_file

    wheels_base = get_wheels_base(args.config)
    wheel_radius = get_wheel_radius(args.config)
    max_n = get_max_n(args.config)
    logger.info(
        f'Wheels base: {wheels_base} '
        f'wheel radius: {wheel_radius} '
        f'max N: {max_n}')

    motion_to_odo = MotionToOdometer(wheel_radius, max_n, wheels_base)

    with open(motion_file_path, 'r') as f:
        data = json.load(f)
    
    logger.info(f"Motion file values: {data}")
    x_arr = data['x']
    y_arr = data['y']

    assert len(x_arr) == len(y_arr), 'ERROR! Sizes should match each other'

    n_left = []
    n_right = []
    phi_arr = []
    phi = 0.0

    x1, y1, phi1 = None, None, pi / 2.0
    n_left = []
    n_right = []
    for i in range(len(x_arr)):
        x2 = x_arr[i]
        y2 = y_arr[i]
        if x1 is None and y1 is None:
            x1, y1 = x2, y2
            continue
        logger.info(
            f'x1: {x1:.2f}, y1: {y1:.2f}, '
            f'x2: {x2:.2f}, y2: {y2:.2f}, '
            f'phi1: {phi1:.3f}')
        angle_n_l, angle_n_r, dist_n, phi2 = motion_to_odo(
            x1, y1,
            phi1,
            x2, y2)
        logger.info(
            f'Angle n left: {angle_n_l:.2f}, '
            f'right: {angle_n_r:.2f}, '
            f'dist n: {dist_n:.2f}, '
            f'phi2: {phi2:.2f}')

        n_left.append(angle_n_l)
        n_right.append(angle_n_r)

        n_left.append(dist_n)
        n_right.append(dist_n)

        x1, y1, phi1 = x2, y2, phi2

    logger.info(f"Saving a result to file")
    with open(res_file_path, 'w') as f:
        json.dump(
            {
                'n_left': n_left,
                'n_right': n_right,
            }, f, indent=4)

if __name__ == "__main__":
    main()
