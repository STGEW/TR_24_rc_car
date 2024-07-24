import json
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation


file_path = '/home/afomin/projects/mj/c/TR_24_rc_car/tools/coordinates.json'

TIME_STEP_SEC = 0.5
SCENE_MARGIN_M = 0.5

# Function to read data from JSON file
def read_data_from_json(filename):
    with open(filename, 'r') as file:
        data = json.load(file)
    return data['x'], data['y'], data['phi'], data['w']


# Function to calculate wheel positions
def calculate_wheel_positions(x, y, angle, wheel_distance):
    half_distance = wheel_distance / 2.0

    # Calculate the positions of the left and right wheels
    x_left = x - half_distance * np.sin(np.pi - angle)
    y_left = y + half_distance * np.cos(np.pi - angle)
    x_right = x + half_distance * np.sin(np.pi - angle)
    y_right = y - half_distance * np.cos(np.pi - angle)

    return x_left, y_left, x_right, y_right

# Read the data from the JSON file
x_array, y_array, angle_array, wheel_distance = read_data_from_json(file_path)

# Set up the figure and axis

fig, ax = plt.subplots()
ax.set_xlim(min(x_array) - SCENE_MARGIN_M, max(x_array) + SCENE_MARGIN_M)
ax.set_ylim(min(y_array) - SCENE_MARGIN_M, max(y_array) + SCENE_MARGIN_M)
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
        x, y, angle, wheel_distance)

    car_line.set_data([x_left, x, x_right], [y_left, y, y_right])
    trace.set_data(x_array[:i+1], y_array[:i+1])
    time_text.set_text(time_template % (i * TIME_STEP_SEC))  # Assuming each step represents 0.1s

    return car_line, trace, time_text

# Create the animation
ani = animation.FuncAnimation(
    fig, animate, frames=len(x_array), init_func=init,
    interval=(1000 * TIME_STEP_SEC), blit=True)

plt.show()