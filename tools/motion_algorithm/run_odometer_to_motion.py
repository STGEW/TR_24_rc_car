import logging
import argparse
import json
from pathlib import Path


from utils import get_wheels_base, get_wheel_radius, get_max_n
from odometer_N_to_motion_algorithm import OdometerToMotion
import differential_vehicle_model


logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[logging.StreamHandler()]
)

logger = logging.getLogger(__name__)


def run_python_algorithm(n_left, n_right, py_odo_to_motion):
    x_arr = []
    y_arr = []
    phi_arr = []
    phi = 0.0
    x = 0.0
    y = 0.0

    for i in range(len(n_left)):
        logger.info(f"Iteration i: {i}")
        n_r = n_right[i]
        n_l = n_left[i]
        
        delta_x, delta_y, phi = py_odo_to_motion(n_r, n_l)
        logger.info(f"Delta x: {delta_x}, y: {delta_y}, phi: {phi}")

        x += delta_x
        y += delta_y

        x_arr.append(x)
        y_arr.append(y)
        phi_arr.append(phi)

    return x_arr, y_arr, phi_arr


def run_cpp_algorithm(n_left, n_right, cpp_odo_to_motion):

    x_arr = []
    y_arr = []
    phi_arr = []
    phi = 0.0
    x = 0.0
    y = 0.0

    for i in range(len(n_left)):
        logger.info(f"Iteration i: {i}")
        n_r = n_right[i]
        n_l = n_left[i]

        position_diff = differential_vehicle_model.Vehicle2DPosition()
        
        cpp_odo_to_motion(n_r, n_l, position_diff)

        delta_x = position_diff.x
        delta_y = position_diff.y
        phi = position_diff.phi
        logger.info(f"Delta x: {delta_x}, y: {delta_y}, phi: {phi}")

        x += delta_x
        y += delta_y

        x_arr.append(x)
        y_arr.append(y)
        phi_arr.append(phi)

    return x_arr, y_arr, phi_arr

def main():
    logger.info('Parsing arguments')
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--config',
        required=True,
        type=Path,
        help='Path to configuration file')
    parser.add_argument(
        '--odometer_file',
        required=True,
        type=Path,
        help="Path to file with odometer values. "
        "Example of the format: {'n_left': [], 'n_right': []}")
    parser.add_argument(
        '--res_file',
        required=True,
        type=Path,
        help='Path to the result file with coordinates'
        "Example of format: {'x': [], 'y': [], 'phi': []}")
    parser.add_argument(
        '--use_python',
        action="store_true",
        help='When the flag is mentioned, python version of the algorithm will be used. '
        'Otherwise a C++ version would be')
    args = parser.parse_args()
    logger.info(f'Arguments are: {args}')
    odometer_file_path = args.odometer_file
    res_file_path = args.res_file

    wheels_base = get_wheels_base(args.config)
    wheel_radius = get_wheel_radius(args.config)
    max_n = get_max_n(args.config)
    logger.info(
        f'Wheels base: {wheels_base} '
        f'wheel radius: {wheel_radius} '
        f'max N: {max_n}')

    with open(odometer_file_path, 'r') as f:
        data = json.load(f)
    
    logger.info(f"Odometer file values: {data}")
    n_left = data['n_left']
    n_right = data['n_right']

    assert len(n_left) == len(n_right), 'ERROR! Sizes should match each other'
    if args.use_python:
        # py
        logger.info("Python version will be running")
        py_odo_to_motion = OdometerToMotion(wheel_radius, max_n, wheels_base)
        x_arr, y_arr, phi_arr = run_python_algorithm(n_left, n_right, py_odo_to_motion)
    else:
        logger.info("C++ version will be running")
        # cpp
        cpp_model = differential_vehicle_model.DifferentialVehicleModel(
            wheels_base, wheel_radius, max_n)
        cpp_odo_to_motion = cpp_model.odometer_to_position_diff
        x_arr, y_arr, phi_arr = run_cpp_algorithm(n_left, n_right, cpp_odo_to_motion)
    
    logger.info(f"Saving a result to file")
    with open(res_file_path, 'w') as f:
        json.dump(
            {
                'x': x_arr,
                'y': y_arr,
                'phi': phi_arr,
            }, f, indent=4)


if __name__ == "__main__":
    main()
