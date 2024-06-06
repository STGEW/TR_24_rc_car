The code is written with FreeRTOS. It utilizes only 1 Core of MCU. The concept of tasks was used to design the system.

### The list of tasks

## motorDriverTask
Takes driverControlData and converts it into PWM signals for motors

## rfRxTask
Responsible for receiving data from tranceiver and sending it to the next task

## imuRawTask
Responsible for communicating with IMU module

## cameraServoControlTask
Responsible for control of the camera for RPI (done for future). 

## commandProcessorTask
Takes received from RC values and converts it into simple data for motorDriverTask. Is reponsible for timeout in case there is no signal from RC.

## main
An entry point, the place where everythin is initialized and triggered.