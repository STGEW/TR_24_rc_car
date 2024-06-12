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
