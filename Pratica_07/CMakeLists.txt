cmake_minimum_required(VERSION 3.13...3.27)

set(PICO_SDK_PATH "/home/david/Documentos/RTOS/pico/pico-sdk")
set(PICO_BOARD "pico_w")

include(pico_sdk_import.cmake)
include(FreeRTOS_Kernel_import.cmake)

project(pratica07 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pratica07
    src/main.c
)

target_include_directories(pratica07 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${CMAKE_CURRENT_LIST_DIR}/src
    ../FreeRTOS-Kernel/include
    ../FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040
)

target_link_libraries(pratica07
    hardware_adc 
    pico_stdlib
    pico_cyw43_arch_none
    pico_stdio_usb
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    
)

pico_enable_stdio_usb(pratica07 1)
pico_enable_stdio_uart(pratica07 0)

pico_add_extra_outputs(pratica07)
