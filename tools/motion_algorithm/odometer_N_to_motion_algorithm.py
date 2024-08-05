from math import pi, sin, cos
import logging


logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[logging.StreamHandler()]
)

# example of data generated for this file
# epochs = 100
# left_wheel_N = [1.1 * max_n for i in range(epochs)]
# right_wheel_N = [1.0 * max_n for i in range(epochs)]

logger = logging.getLogger(__name__)


class OdometerToMotion:
    def __init__(self, wheel_radius, max_n, wheels_base):
        self._wheel_radius = wheel_radius
        self._max_n = max_n
        self._wheels_base = wheels_base
        self._phi_prev = 0.0
        self._phi = 0.0
        logger.info(
            f'Wheel radius: "{self._wheel_radius}" '
            f'max N: "{self._max_n}" '
            f'wheels base: "{self._wheels_base}"')
    
    def __call__(self, n_r, n_l):
        """
        n_r (int) - count of holes on the right side
        n_l (int) - count of holes on the left side
        return:
            x (float), y (float) - meters, movement of the vehicle
            phi (float) - radians, orientation of the vehicle
        """
        logger.info(f'n_r: {n_r} n_l: {n_l}')

        delta_d_r = self._calc_delta_d(n_r)
        delta_d_l = self._calc_delta_d(n_l)
        logger.info(f'delta d r: {delta_d_r} l: {delta_d_l}')

        turn_radius = self._calc_turning_radius(
            delta_d_r,
            delta_d_l)
        logger.info(f'turn radius: {turn_radius}')

        delta_phi = self._calc_delta_phi(
            delta_d_r, delta_d_l, turn_radius)
        self._phi_prev = self._phi
        self._phi += delta_phi

        logger.info(f'phi: {self._phi} delta phi: {delta_phi}')

        x = self._calc_x(
            delta_d_r,
            delta_d_l,
            self._phi_prev,
            self._phi,
            turn_radius)

        y = self._calc_y(
            delta_d_r,
            delta_d_l,
            self._phi_prev,
            self._phi,
            turn_radius)

        return x, y, self._phi
        
    def _calc_delta_d(self, delta_n):
        # delta_n (int) - a value which represents a turn of wheel
        # wheel_radius (float) - a wheel radius in meters
        # max_n (int) - count of holes in odometer
        # return:
        # delta_d (float) - the length of the path in meters
        delta_n = float(delta_n)
        return delta_n * 2 * pi * self._wheel_radius / self._max_n


    # can be called only when delta_d_r != delta_d_l
    def _calc_turning_radius(self, delta_d_r, delta_d_l):
        # delta_d_r (float) - meters of movement right side
        # delta_d_l (float) - meters of movement left side
        # wheels_base (float) - a distance between wheels in m on vehicle
        # return:
        # turning radius (float) - meters
        if delta_d_l == delta_d_r:
            return float('inf')
        return delta_d_r * self._wheels_base / (delta_d_l - delta_d_r)

    # calc a change in a direction angle
    def _calc_delta_phi(self, delta_d_r, delta_d_l, turn_radius):
        # delta_d_r (float) - meters of movement right side
        # delta_d_l (float) - meters of movement left side
        # turn_radius (float) - meters, a turning radius
        # return
        # delta_phi (float) - rad, a diff in angle for delta
        if delta_d_l == delta_d_r:
            return 0.0
        # delta_d_l != delta_d_r
        if turn_radius == 0.0:
            return delta_d_l / self._wheels_base
        else:
            # turn_radius != 0.0
            return delta_d_r / turn_radius

    # at i interval
    def _calc_x(self, delta_d_r, delta_d_l, phi_prev, phi, turn_radius):
        # delta_d_r (float) - meters of movement right side
        # delta_d_l (float) - meters of movement left side
        # phi_prev (float) - radians, at i-1 of an angle
        # phi (float) - radians, at i of an angle
        # turn_radius (float) - meters, a turning radius
        # wheels_base (float) - meters, distance between wheels
        # return
        # x at interval i (float)
        if delta_d_r == delta_d_l:
            return sin(phi) * (delta_d_r + delta_d_l) / 2.0
        return (turn_radius + self._wheels_base / 2.0) * (
                sin(phi_prev + pi / 2.0) + sin(phi - pi / 2.0))

    def _calc_y(self, delta_d_r, delta_d_l, phi_prev, phi, turn_radius):
        # delta_d_r (float) - meters of movement right side
        # delta_d_l (float) - meters of movement left side
        # phi_prev (float) - radians, at i-1 of an angle
        # phi (float) - radians, at i of an angle
        # turn_radius (float) - meters, a turning radius
        # wheels_base (float) - meters, distance between wheels
        # return
        # y at interval i (float)
        if delta_d_r == delta_d_l:
            return cos(phi) * (delta_d_r + delta_d_l) / 2.0
        return (turn_radius + self._wheels_base / 2.0) * (
                cos(phi_prev + pi / 2.0) + cos(phi - pi / 2.0))
