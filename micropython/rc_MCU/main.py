import utime
from micropython import const
from time import sleep
from machine import UART, Pin
import os

from joystick_task import joystick_task
from rf_tx_task import rf_tx_task
from nrf24l01test import initiator

# because we want to duplicate prints to uart
uart0 = UART(0, baudrate=115200, tx=Pin(0), rx=Pin(1))
os.dupterm(uart0, 0)

while True:
    # initiator()

    joystick_x, joystick_y = joystick_task()
    joystick_x = 343
    joystick_y = 4535
    print(f'joystick: {joystick_x} {joystick_y}')
    rf_tx_task(joystick_x, joystick_y)
    # utime.sleep_ms(const(250))
    sleep(0.01)
