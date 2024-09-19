from consts import X_AXIS_PIN, Y_AXIS_PIN
from machine import ADC, Pin


adc_x = ADC(Pin(X_AXIS_PIN)) # 0-65535
adc_y = ADC(Pin(Y_AXIS_PIN)) # 0-65535


def run_joystick_task():
    x = adc_x.read_u16()
    y = adc_y.read_u16()
    return x, y
