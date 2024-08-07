# Tools to help test commands parser

## Python part

### Command line tool
This tool uses original C++ code of the commandsParser and allow you to communicate over UART using the protocol.

### How to run
<code>
pip install pyserial
python commands_parser_cli.py --port /dev/ttyUSB0 --br 115200
python commands_parser_cli.py --port /dev/ttyUSB1 --br 115200 --send
</code>


### Command interface

1) To set next point on the path use format: point <x float> <y float>
<code>
point 3.000 4.000
printf "\x54\x52\x08\x3f\x80\x00\x00\x40\x00\x00\x00" > /dev/ttyUSB1
</code>

2) to stop current task:
<code>
stop
</code>


## How to build everything
1) add soft links for differentialVehicleModel.cpp and differentialVehicleModel.h to the current directory
<code>
cd commands_parser
ln -s ../../freertos_code/car_MCU/commandsParser.h commandsParser.h
ln -s ../../freertos_code/car_MCU/commandsParser.cpp commandsParser.cpp
</code>

2) build commands_parser.so library:
<code>
mkdir build
cd build
cmake ..
make
cd ../
mv build/libcommands_parser.so ./commands_parser.so
</code>