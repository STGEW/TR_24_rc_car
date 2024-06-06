The code is written with FreeRTOS. It utilizes only 1 Core of MCU. The concept of tasks was used to design the system.

### The list of tasks

## joystickTask
Read data from joystick (ADC is used)

## rfTxTask
Responsible for transmitting the data with tranceiver

## main
An entry point, the place where everythin is initialized and triggered.
