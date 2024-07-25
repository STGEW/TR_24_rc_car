What's inside:
1) manual motion part - it takes count of interruptions from odometer and converts it to the new distance of the car. Two files can be used: 

manual_motion_algorithm.ipynb - an algorithm that will calculate the distances and phi angle and store it to json file

manual_motion_plotter.py - a plotter, it will read json file and plot it with matplotlib

Files share knowledge with the help of the json file. Path to that file should be mentioned in the code.

2) auto motion part - it knows desired position of the vehicle and should transfer it into 2 values: count of holes to rotate the vehicle, count of holes to move the vehicle along the straight line.

auto_motion_algorithm.ipynb - an algorithm that will calculate the distances and phi angle and store it to json file

auto_motion_plotter.py - a plotter, it will read json file and plot it with matplotlib
 