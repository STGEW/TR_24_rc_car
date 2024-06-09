from consts import X_AXIS_PIN, Y_AXIS_PIN
from machine import ADC, Pin


adc_x = ADC(Pin(X_AXIS_PIN)) # 0-65535
adc_y = ADC(Pin(Y_AXIS_PIN)) # 0-65535


class Joystick:
    x_axis = 0
    y_axis = 0


joystick = Joystick()


def joystick_task():
    joystick.x = adc_x.read_u16()
    joystick.y = adc_y.read_u16()
    return joystick
