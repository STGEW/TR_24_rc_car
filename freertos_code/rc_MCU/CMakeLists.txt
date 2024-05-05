cmake_minimum_required(VERSION 3.13)

# my change
set( FREERTOS_KERNEL_PATH "../../../../../../Source" )


# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

# RF24 library
include(./lib/RF24/CMakeLists.txt)

project(rc_MCU C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

pico_sdk_init()

add_executable(main_rc_mcu
        main.cpp
        joystickTask.cpp
        rfTxTask.cpp
        IntQueueTimer.c
        RegTest.s
        ../../../../Common/Minimal/blocktim.c
        ../../../../Common/Minimal/countsem.c
        ../../../../Common/Minimal/dynamic.c
        ../../../../Common/Minimal/recmutex.c
        ../../../../Common/Minimal/QueueOverwrite.c
        ../../../../Common/Minimal/EventGroupsDemo.c
        ../../../../Common/Minimal/IntSemTest.c
        ../../../../Common/Minimal/IntQueue.c
        ../../../../Common/Minimal/TaskNotify.c
        ../../../../Common/Minimal/TimerDemo.c
        ../../../../Common/Minimal/GenQTest.c
        ../../../../Common/Minimal/death.c
        ../../../../Common/Minimal/semtest.c
        ../../../../Common/Minimal/BlockQ.c
        ../../../../Common/Minimal/flop.c
        )

target_compile_definitions(main_rc_mcu PRIVATE
        mainCREATE_SIMPLE_BLINKY_DEMO_ONLY=0
        )

target_include_directories(main_rc_mcu PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../../../../Common/include)

target_compile_definitions(main_rc_mcu PRIVATE
        PICO_STDIO_STACK_BUFFER_SIZE=64 # use a small printf on stack buffer
)
target_link_libraries(main_rc_mcu pico_stdlib hardware_adc hardware_spi RF24 FreeRTOS-Kernel FreeRTOS-Kernel-Heap4)
pico_add_extra_outputs(main_rc_mcu)

# Use USB uart
pico_enable_stdio_usb(main_rc_mcu 1)
pico_enable_stdio_uart(main_rc_mcu 1)
