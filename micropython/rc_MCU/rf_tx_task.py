from consts import CE_PIN, CSN_PIN
from machine import Pin, SPI


csn = Pin(CSN_PIN, mode=Pin.OUT, value=1)
ce = Pin(CE_PIN, mode=Pin.OUT, value=0)

nrf = NRF24L01(SPI(0), csn, ce, channel=100, payload_size=32)

# Addresses are in little-endian format. They correspond to big-endian
# 0xf0f0f0f0e1, 0xf0f0f0f0d2
pipes = (b"\xe1\xf0\xf0\xf0\xf0", b"\xd2\xf0\xf0\xf0\xf0")

nrf.stop_listening()
    
nrf.open_tx_pipe(pipes[0])


def rf_tx_task():
    millis = utime.ticks_ms()
    try:
        nrf.send(struct.pack("i", millis))
    except OSError:
        print("OS error")
        pass
