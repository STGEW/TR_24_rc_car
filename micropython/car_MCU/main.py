
from consts import FORWARD, OFF, REVERSE, BRAKE, Driver
import utime
from micropython import const
from machine import UART, Pin
import os
from time import sleep

# from motor_driver_task import init_motor_driver_task, motor_driver_task
# from rf_rx_task import rf_rx_task
from nrf24l01test import responder

uart0 = UART(0, baudrate=115200, tx=Pin(0), rx=Pin(1))
os.dupterm(uart0, 0)

# driver = Driver()
# driver.direction_A = FORWARD
# driver.direction_B = FORWARD
# init_motor_driver_task()

while True:
    responder()
    # res = rf_rx_task()
    # if res:
    #     joystick_x, joystick_y = res
    #     print(f"joystick x: {joystick_x} y: {joystick_y}")
    # print(f'driver: {driver.direction_A} {driver.direction_B} {driver.duty_cycle_A} {driver.duty_cycle_B}')

    # if driver.duty_cycle_A > 60000:
    #     if driver.direction_A == FORWARD:
    #         driver.direction_A = REVERSE
    #     else:
    #         driver.direction_A = FORWARD
    #     driver.duty_cycle_A = 0
    
    # driver.duty_cycle_A += 2000

    # if driver.duty_cycle_B > 60000:
    #     if driver.direction_B == FORWARD:
    #         driver.direction_B = REVERSE
    #     else:
    #         driver.direction_B = FORWARD
    #     driver.duty_cycle_B = 0
    
    # driver.duty_cycle_B += 2000

    # motor_driver_task(driver)
    sleep(0.01)
