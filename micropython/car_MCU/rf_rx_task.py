from consts import CE_PIN, CSN_PIN
from machine import Pin, SPI


csn = Pin(CSN_PIN, mode=Pin.OUT, value=1)
ce = Pin(CE_PIN, mode=Pin.OUT, value=0)

nrf = NRF24L01(SPI(0), csn, ce, channel=100, payload_size=32)

# Addresses are in little-endian format. They correspond to big-endian
# 0xf0f0f0f0e1, 0xf0f0f0f0d2
pipes = (b"\xe1\xf0\xf0\xf0\xf0", b"\xd2\xf0\xf0\xf0\xf0")

nrf.open_rx_pipe(1, pipes[0])

nrf.start_listening()

def rf_rx_task():
    if nrf.any():
        while nrf.any():
            buf = nrf.recv()
            millis = struct.unpack("i", buf)
            print("received", millis)

def test_master():
    nrf.stop_listening()
    
    nrf.open_tx_pipe(pipes[0])
    while True:
        millis = utime.ticks_ms()
        print("sending ", millis)
        pin.value(1)
        try:
            nrf.send(struct.pack("i", millis))
        except OSError:
            print("OS error")
            pass

        pin.value(0)
                    
        # delay then loop
        utime.sleep_ms(500)

