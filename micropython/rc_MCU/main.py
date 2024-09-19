from time import ticks_diff, ticks_us
from machine import UART, Pin
import os

from consts import UNDER_DEBUG
from consts import LOOP_PERIOD_USEC

from joystick_task import run_joystick_task
from rf_tx_task import init_rf_tx_task, run_rf_tx_task

# not esp
# uart0 = UART(0, baudrate=115200, tx=0, rx=1)
# esp
uart0 = UART(2, baudrate=115200, timeout_char=100)


def init_uart():
    # os.dupterm(uart0, 0)
    pass


def init():
    print(f'Run init')
    init_uart()


def loop():
    last_t = ticks_us()
    x, y = run_joystick_task()
    if UNDER_DEBUG:
        data = uart0.readline()
        if data:
            # print(f"Read from UART data: {data}")
            x, y = data.decode('utf-8').split(' ')
            x = int(x)
            y = int(y)
    print(f'joystick: {x} {y}')
    run_rf_tx_task(x, y)
    return last_t


def main():
    init()
    # variable to track loop call
    last_t = ticks_us()

    while True:
        cur_t = ticks_us()
        if ticks_diff(cur_t, last_t) > LOOP_PERIOD_USEC:
            last_t = loop()

main()
