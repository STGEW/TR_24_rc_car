### Prepare environment

1) Install dependencies (example is for Ubuntu)
```shell
sudo apt install gcc-arm-none-eabi
```

2) Clone freertos repo
```shell
cd /home/$USER/build/
git clone https://github.com/FreeRTOS/FreeRTOS.git
cd ./FreeRTOS
git submodule init
git submodule update --recursive
```

3) clone pico SDK (please use this document as a reference - https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
```shell
mkdir /home/$USER/build
cd /home/$USER/build
mkdir ./pico
cd ./pico
git clone https://github.com/raspberrypi/pico-sdk.git --branch master
cd pico-sdk
git submodule init
git submodule update --recursive
cd ..
git clone https://github.com/raspberrypi/pico-examples.git --branch master
```

4) clone and build nRF24 driver (please use this document as a reference - https://nrf24.github.io/RF24/md_docs_2pico__sdk.html)
```shell
cd /home/$USER/build
git clone https://github.com/nRF24/RF24.git

export PICO_SDK_PATH=/home/$USER/build/pico/pico-sdk
cmake ../examples_pico -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico
cmake --build . --config Release
```

### How to build freertos C code
Inside freertos_code/car_MCU or freertos_code/rc_MCU
```shell
mkdir build
cd build
cmake ..
make
```

### Upload firmware to MCU
power off
press the boot btn
power on
you'll see in your file system a RPI storage
just drag and drop build/.uf2 file to that storage
MCU will automatically reboot and ready to go
