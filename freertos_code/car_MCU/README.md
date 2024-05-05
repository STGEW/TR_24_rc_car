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


1) не очень отзывчив робот, почему? не похоже, что с частотой 10 Гц обрабатывается передатчик, может быть попробовать приоритет поднять этим таскам?
задержка примерно 1 секунда.
Нужно наверное замерить, сколько времени тратится на каждый таск, грубо говоря сколько времени уходит на одну итерацию

2) добавляем IMU модуль?