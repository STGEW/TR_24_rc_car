import logging
import argparse
from pathlib import Path
import json
from math import pi

from utils import get_wheels_base, get_wheel_radius, get_max_n
from motion_to_odometer_algorithm import MotionToOdometer
import differential_vehicle_model


logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[logging.StreamHandler()]
)


logger = logging.getLogger(__name__)


def run_python_algorithm(x_arr, y_arr, py_motion_to_odo):
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
        angle_n_l, angle_n_r, dist_n, phi2 = py_motion_to_odo(
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
    return n_left, n_right


def run_cpp_algorithm(x_arr, y_arr, cpp_motion_to_odo):

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
        path = differential_vehicle_model.PathChunk()
        path.x1 = x1
        path.y1 = y1
        path.phi1 = phi1
        path.x2 = x2
        path.y2 = y2

        odo = differential_vehicle_model.OdometerValues()
        cpp_motion_to_odo(path,odo)
        angle_n_l = odo.angle_n_l
        angle_n_r = odo.angle_n_r
        dist_n = odo.dist_n
        phi2 = odo.phi2
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
    return n_left, n_right

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
    parser.add_argument(
        '--use_python',
        action="store_true",
        help='When the flag is mentioned, python version of the algorithm will be used. '
        'Otherwise a C++ version would be')
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

    with open(motion_file_path, 'r') as f:
        data = json.load(f)
    
    logger.info(f"Motion file values: {data}")
    x_arr = data['x']
    y_arr = data['y']

    assert len(x_arr) == len(y_arr), 'ERROR! Sizes should match each other'

    if args.use_python:
        # py
        logger.info("Python version will be running")
        py_motion_to_odo = MotionToOdometer(wheel_radius, max_n, wheels_base)
        n_left, n_right = run_python_algorithm(x_arr, y_arr, py_motion_to_odo)
    else:
        logger.info("C++ version will be running")
        # cpp
        cpp_model = differential_vehicle_model.DifferentialVehicleModel(
            wheels_base, wheel_radius, max_n)
        cpp_motion_to_odo = cpp_model.path_to_odometer_values
        n_left, n_right = run_cpp_algorithm(x_arr, y_arr, cpp_motion_to_odo)

    logger.info(f"Saving a result to file")
    with open(res_file_path, 'w') as f:
        json.dump(
            {
                'n_left': n_left,
                'n_right': n_right,
            }, f, indent=4)


if __name__ == "__main__":
    main()
