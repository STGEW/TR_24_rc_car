pip install mpremote

useful commands:
mpremote --help
mpremote a1

mpremote a1 cp ./rc_MCU/joystick_task.py :
mpremote a1 cp ./rc_MCU/rf_tx_task.py :
mpremote a1 cp ./rc_MCU/main.py :
mpremote a1 cp ./consts.py :

mpremote a1 cp ./car_MCU/motor_driver_task.py :
mpremote a1 cp ./car_MCU/rf_rx_task.py :
mpremote a1 cp ./car_MCU/main.py :
mpremote a1 cp ./consts.py :

mpremote a1 cp ./nrf24l01test.py :

to put to the MCU driver for NRF
git clone https://github.com/micropython/micropython-lib.git
cd micropython-lib/micropython/drivers/radio/nrf24l01
mpremote a1 cp ./nrf24l01.py :

(micropy_env) (base) afomin@afomin-laptop:~/projects/mj/TR_24_rc_car$ mpremote a1 ls
ls :
         391 consts.py
         220 joystick_task.py
         265 main.py

# to upload files
cd micropython/car_MCU
ampy -p /dev/ttyUSB0 -b 115200 put ../consts.py && \
ampy -p /dev/ttyUSB0 -b 115200 put main.py && \
ampy -p /dev/ttyUSB0 -b 115200 put motor_driver_task.py && \
ampy -p /dev/ttyUSB0 -b 115200 put rf_rx_task.py


find ./ -name nrf24l01.py
ampy -p /dev/ttyUSB0 -b 115200 put ./micropython-lib/micropython/drivers/radio/nrf24l01/nrf24l01.py

# ESP32 version
ampy -p /dev/ttyUSB1 -b 115200 put ../consts.py && \
ampy -p /dev/ttyUSB1 -b 115200 put main.py && \
ampy -p /dev/ttyUSB1 -b 115200 put motor_driver_task.py && \
ampy -p /dev/ttyUSB1 -b 115200 put utils.py && \
ampy -p /dev/ttyUSB1 -b 115200 put rf_rx_task.py && \
ampy -p /dev/ttyUSB1 -b 115200 put $MICROLIB_PATH/micropython/drivers/radio/nrf24l01/nrf24l01.py

ampy -p /dev/ttyUSB1 -b 115200 put ../consts.py && \
ampy -p /dev/ttyUSB1 -b 115200 put main.py && \
ampy -p /dev/ttyUSB1 -b 115200 put joystick_task.py && \
ampy -p /dev/ttyUSB1 -b 115200 put rf_tx_task.py && \
ampy -p /dev/ttyUSB1 -b 115200 put $MICROLIB_PATH/micropython/drivers/radio/nrf24l01/nrf24l01.py


esptool.py --chip esp32 --port /dev/ttyUSB1 --baud 460800 erase_flash && \
esptool.py --chip esp32 --port /dev/ttyUSB1 --baud 460800 write_flash -z 0x1000 ~/Downloads/ESP32_GENERIC-20230426-v1.20.0.bin