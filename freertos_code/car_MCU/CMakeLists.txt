cmake_minimum_required(VERSION 3.13)

# setup env variables
set( FREERTOS_KERNEL_PATH "/home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Source" )
set( PICO_SDK_PATH "/home/$ENV{USER}/build/pico/pico-sdk" )

# Pull in SDK (must be before project)
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Pull in FreeRTOS
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

# RF24 library
include(/home/$ENV{USER}/build/RF24/CMakeLists.txt)

project(car_MCU C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

pico_sdk_init()

add_executable(main_car_mcu
        main.cpp
        motorDriverTask.cpp
        commandsProcessorTask.cpp
        cameraServoControlTask.cpp
        imuRawTask.cpp
        rfRxSimulatorTask.cpp
        rfRxTask.cpp
        IntQueueTimer.c
        RegTest.s
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/blocktim.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/countsem.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/dynamic.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/recmutex.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/QueueOverwrite.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/EventGroupsDemo.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/IntSemTest.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/IntQueue.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/TaskNotify.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/TimerDemo.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/GenQTest.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/death.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/semtest.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/BlockQ.c
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/Minimal/flop.c
        )

target_compile_definitions(main_car_mcu PRIVATE
        mainCREATE_SIMPLE_BLINKY_DEMO_ONLY=0
        )

target_include_directories(main_car_mcu PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        /home/$ENV{USER}/build/FreeRTOS/FreeRTOS/Demo/Common/include)

target_compile_definitions(main_car_mcu PRIVATE
        PICO_STDIO_STACK_BUFFER_SIZE=64 # use a small printf on stack buffer
)
target_link_libraries(main_car_mcu pico_stdlib hardware_pwm hardware_spi hardware_uart hardware_i2c RF24 FreeRTOS-Kernel FreeRTOS-Kernel-Heap4)
pico_add_extra_outputs(main_car_mcu)

# Use USB uart
pico_enable_stdio_usb(main_car_mcu 1)
pico_enable_stdio_uart(main_car_mcu 1)
