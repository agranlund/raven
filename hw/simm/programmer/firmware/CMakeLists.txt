cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(ravenprog C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(ravenprog
    main.c
    flash.c
)

pico_add_extra_outputs(ravenprog)

target_link_libraries(ravenprog pico_stdlib)

pico_enable_stdio_usb(ravenprog 1)
pico_enable_stdio_uart(ravenprog 0)

