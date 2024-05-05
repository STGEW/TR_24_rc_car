остановился на своем примере с управлением двигателями. Я все переименовал. Надо опять сделать нормально.
Из больших задач - разобраться в синтаксисе С
Посмотреть что-то про cmake, очень уж непонятно все

```shell
mkdir build
cd build
cmake ..
make
```


where to find related to MCU C things



https://github.com/ebrezadev/nRF24L01-C-Driver

https://nrf24.github.io/RF24/md_docs_pico_sdk.html
cd RF24
mkdir build
cd build

export PICO_SDK_PATH=/home/afomin/projects/mj/freertos_projects/pico-sdk

cmake ../examples_pico -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico
cmake --build . --config Release
