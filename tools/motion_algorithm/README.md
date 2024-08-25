#### Build a library from cpp files
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

#### Run unit tests
<code>python ./unit_tests.py</code>
