from consts import X_AXIS_PIN, Y_AXIS_PIN
from machine import ADC, Pin


adc_x = ADC(Pin(X_AXIS_PIN)) # 0-65535
adc_y = ADC(Pin(Y_AXIS_PIN)) # 0-65535


def joystick_task():
    return adc_x.read_u16(), adc_y.read_u16()
