# Tools to help with a motion algorithm

## Python part

### Algorithms Python

#### Motion to odometer
<code>motion_to_odometer_algorithm.py</code> - an algorithm that suggests for change in position the count of interruptions from which and right odometer. The movement is executed in 2 steps.
First one - to change a yaw angle, the second one to change the position in a simple along the line movement.

#### Odometer to motion
<code>odometer_N_to_motion_algorithm.py</code> - an algorithm that calculate the difference in position of the vehicle from data from odometers.
<code>Input vector: {n_r, n_l}</code> - values from right and left odometer
<code>Output vector: {x, y, phi}</code> - diff  in coordinates of the vehicle in meters and yaw angle in radians

### Algorithms C++
Algorithms are stored within freertos_code/car_MCU. There are two files:
<code>differentialVehicleModel.h
differentialVehicleModel.cpp</code>

#### How to test C++ part with Python
1) add soft links for differentialVehicleModel.cpp and differentialVehicleModel.h to the current directory
<code>
cd motion_algorithm
ln -s ../../freertos_code/car_MCU/differentialVehicleModel.h differentialVehicleModel.h
ln -s ../../freertos_code/car_MCU/differentialVehicleModel.cpp differentialVehicleModel.cpp
</code>

2) build differential_vehicle_model.so library:
<code>
mkdir build
cd build
cmake .. && make
mv libdifferential_vehicle_model.so ../differential_vehicle_model.so
</code>


### Python wrappers to work with algorithms

#### Run motion to odometer
<code>run_motion_to_odometer.py</code>
<code>python ./run_motion_to_odometer.py --config data/config.yml --motion_file data/path_2.json --res_file ~/projects/tmp/odometer_cpp.json</code>
<code>python ./run_motion_to_odometer.py --config data/config.yml --motion_file data/path_2.json --res_file ~/projects/tmp/odometer_py.json --use_python</code>

#### Run odometer to motion
<code>run_odometer_to_motion.py</code>
<code>python ./run_odometer_to_motion.py --config data/config.yml --odometer_file ~/projects/tmp/odometer_cpp.json --res_file ~/projects/tmp/motion_cpp.json</code>
<code>python ./run_odometer_to_motion.py --config data/config.yml --odometer_file ~/projects/tmp/odometer_py.json --res_file ~/projects/tmp/motion_py.json --use_python</code>

#### Plot and compare results visually
<code>python ./plot_pathes.py --path1 data/path_2.json --path2 ~/projects/tmp/motion_cpp.json --name cpp</code>
<code>python ./plot_pathes.py --path1 data/path_2.json --path2 ~/projects/tmp/motion_py.json --name py</code>

#### Run unit tests
<code>python ./unit_tests.py</code>