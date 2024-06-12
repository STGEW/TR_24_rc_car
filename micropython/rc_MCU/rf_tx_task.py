from consts import CE_PIN, CSN_PIN
from machine import Pin, SPI
from nrf24l01 import NRF24L01
import struct

csn = Pin(CSN_PIN, mode=Pin.OUT, value=1)
ce = Pin(CE_PIN, mode=Pin.OUT, value=0)

spi = SPI(0, sck=Pin(18), mosi=Pin(19), miso=Pin(16))
nrf = NRF24L01(spi, csn, ce, payload_size=8)

# Addresses are in little-endian format. They correspond to big-endian
# 0xf0f0f0f0e1, 0xf0f0f0f0d2
pipes = (b"\xe1\xf0\xf0\xf0\xf0", b"\xd2\xf0\xf0\xf0\xf0")


nrf.stop_listening()
    
nrf.open_tx_pipe(pipes[0])


def rf_tx_task(joystick_x, joystick_y):
    try:
        nrf.send(struct.pack("ii", joystick_x, joystick_y))
    except OSError:
        print("OS error")
        pass
