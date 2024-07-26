import json
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import argparse
from pathlib import Path
import logging

from utils import get_wheels_base, get_timestep_sec, get_scene_margin_m


logger = logging.getLogger(__name__)


# Function to read data from JSON file
def read_data_from_file(filename):
    with open(filename, 'r') as file:
        data = json.load(file)
    return data['x'], data['y'], data['phi']


# Function to calculate wheel positions
def calculate_wheel_positions(x, y, angle, wheels_base):
    half_distance = wheels_base / 2.0

    # Calculate the positions of the left and right wheels
    x_left = x - half_distance * np.sin(np.pi - angle)
    y_left = y + half_distance * np.cos(np.pi - angle)
    x_right = x + half_distance * np.sin(np.pi - angle)
    y_right = y - half_distance * np.cos(np.pi - angle)

    return x_left, y_left, x_right, y_right


def plot(file_with_coords, timestep_sec, scene_margin_m, wheels_base):
    # Read the data from the JSON file
    x_array, y_array, angle_array = read_data_from_file(file_with_coords)

    # Set up the figure and axis

    fig, ax = plt.subplots()
    ax.set_xlim(min(x_array) - scene_margin_m, max(x_array) + scene_margin_m)
    ax.set_ylim(min(y_array) - scene_margin_m, max(y_array) + scene_margin_m)
    ax.set_aspect('equal')
    ax.grid()

    # Plot elements
    car_line, = ax.plot([], [], 'o-', lw=2)
    trace, = ax.plot([], [], 'r.', lw=1, ms=2)
    time_template = 'time = %.1fs'
    time_text = ax.text(0.05, 0.9, '', transform=ax.transAxes)

    # Initialization function for animation
    def init():
        car_line.set_data([], [])
        trace.set_data([], [])
        time_text.set_text('')
        return car_line, trace, time_text

    # Animation function
    def animate(i):
        x = x_array[i]
        y = y_array[i]
        angle = angle_array[i]

        x_left, y_left, x_right, y_right = calculate_wheel_positions(
            x, y, angle, wheels_base)

        car_line.set_data([x_left, x, x_right], [y_left, y, y_right])
        trace.set_data(x_array[:i+1], y_array[:i+1])
        time_text.set_text(time_template % (i * timestep_sec))  # Assuming each step represents 0.1s

        return car_line, trace, time_text

    # Create the animation
    ani = animation.FuncAnimation(
        fig, animate, frames=len(x_array), init_func=init,
        interval=(1000 * timestep_sec), blit=True)
    plt.show()


def main():
    logger.info('Parsing arguments')
    parser = argparse.ArgumentParser()
    parser.add_argument(
        'config', type=Path, help='Path to configuration file')
    parser.add_argument(
        'coord_file', type=Path,
        help="Path to file with coordinates. "
        "Example of format: {'x': [], 'y': [], 'phi': []}")
    args = parser.parse_args()
    logger.info(f'Arguments are: {args}')

    wheels_base = get_wheels_base(args.config)
    timestep_sec = get_timestep_sec(args.config)
    scene_margin_m = get_scene_margin_m(args.config)
    logger.info(
        f'Wheels base: {wheels_base}')
    
    plot(args.coord_file, timestep_sec, scene_margin_m, wheels_base)

if __name__ == "__main__":
    main()
