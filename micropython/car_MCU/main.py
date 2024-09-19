from machine import UART
import os
from time import ticks_diff, ticks_us, ticks_ms


from consts import LOOP_PERIOD_USEC
from consts import EnginesPwr, Driver
from consts import UNDER_DEBUG
from consts import RF_TIMEOUT_MSEC

from motor_driver_task import init_motor_driver_task, run_motor_driver_task
from rf_rx_task import init_rf_rx_task, run_rf_rx_task

from utils import conv_joy_to_engines_pwr
from utils import conv_engines_pwr_to_driver
from utils import timeout_protection


# not esp
# uart0 = UART(0, baudrate=115200, tx=0, rx=1)
# esp
uart0 = UART(2, baudrate=115200, timeout_char=100)
last_rf_rx_tick = None

def init_uart():
    # os.dupterm(uart0, 0)
    pass
 

def init():
    print(f'Run init')
    init_uart()
    init_rf_rx_task()
    init_motor_driver_task()


def loop(last_rf_rx_tick):
    '''
    Arguments:
    - last_rf_rx_tick - last usec tick when the package from RF
       was received
    Return:
    - last_t - usec tick when loop was called. Required to track
       frequency for loop method
    - last_rf_rx_tick - msec tick whel last RF package arrived
    '''
    last_t = ticks_us()
    j_x, j_y = run_rf_rx_task()

    if UNDER_DEBUG:
        data = uart0.readline()
        if data:
            # print(f"Read from UART data: {data}")
            j_x, j_y = data.decode('utf-8').split(' ')
            j_x = int(j_x)
            j_y = int(j_y)
            # print(f"Joystick x: {j_x}, y: {j_y}")
    
    driver = Driver()
    # print(f'Driver initial values: {driver.direction_A} {driver.direction_B} {driver.duty_cycle_A} {driver.duty_cycle_B}')
    if j_x is not None and j_y is not None:
        last_rf_rx_tick = ticks_ms()
        eng_pwr = EnginesPwr()
        conv_joy_to_engines_pwr(j_x, j_y, eng_pwr)
        print(f"Engine pwr: {eng_pwr.left} {eng_pwr.right}")
        conv_engines_pwr_to_driver(eng_pwr, driver)
        print(f'Driver: {driver.direction_A} {driver.direction_B} {driver.duty_cycle_A} {driver.duty_cycle_B}')
        run_motor_driver_task(driver)
    protection = timeout_protection(driver, last_rf_rx_tick)
    if protection is True:
        # stop the vehicle
        run_motor_driver_task(driver)
    return last_t, last_rf_rx_tick


def main():
    init()
    # variable to track loop call
    last_t = ticks_us()
    # variable to track last received package from RF
    last_rf_rx_tick = ticks_ms() - RF_TIMEOUT_MSEC

    while True:
        cur_t = ticks_us()
        if ticks_diff(cur_t, last_t) > LOOP_PERIOD_USEC:
            last_t, last_rf_rx_tick = loop(last_rf_rx_tick)

main()
