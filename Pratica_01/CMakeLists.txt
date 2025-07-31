cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(pratica1 C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pratica1
    pratica1.c
)

target_link_libraries(pratica1 pico_stdlib pico_cyw43_arch_none pico_stdio_usb)

# enable usb output, disable uart output
pico_enable_stdio_usb(pratica1 1)
pico_enable_stdio_uart(pratica1 0)

pico_add_extra_outputs(pratica1)

