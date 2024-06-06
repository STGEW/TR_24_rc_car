# What's inside?
It's a project with a self build RC car. The project doesn't pretend to be a production ready, but is done for educational purposes.

It has two major parts - a car itself + remote controller.
The project covers:
1) a hardware design of the car and RC - PCB, components, etc
2) a software done in C (TBH it's C++, but only a tiny set of features was used) and in Micropython. They are interchangable and should work on the same set of the hardware.

# Car
A car is built on the platform ordered on the well known web site. The platform has frame done with plexiglass + 4 motos with wheels.
TBD - insert an image

# RC
RC is build with 2 joysticks. Each joystick can be moved in one direction (for the convenience).
TBD - insert an image

# Communication
Communication is done with 2 tranceivers via RF channel. A protocol is extremely simple.

# Freertos part
The code is written on C with Freertos OS. For technical details please check ./freertos_code/README.md

# Micropython part
TBD

# BOM
Joysticks - name
Battery for car - LiPo battery
Batteries for RC - 4 AA type batteries. I'm using 4 Ni-MH accumulators. Can be recharged.
DC-DC converter - MINI_360. You need 3 of them for converting input voltage to 5V or 3.3V
Motor driver - TBD
MCU - 2 items. RP2040
Motors - 4 of them, they are in a set with car frame

# diagram
Diagram done with draw.io explains in a user-friendly way how everything is connected together.
TBD - should be merged into 1 diagram with actual pins

# PCB
Done with a Kicad. Please check ./pcb_design/car_pcb directory with project. ./pcb_design/to_factory contains gerber files generated from the project.
TBD - inser couple of images

# 3D models
TBD - insert couple of images

# Resources
Contains useful pdf files and images


TBD:
- for freertos code
	1) put pins into one place
	2) clean the code
- fix problem with PCB (make rev 1.2)
- make a generic draw.io diagram with valid pins
