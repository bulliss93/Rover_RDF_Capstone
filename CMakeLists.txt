cmake_minimum_required(VERSION 3.19)

include(pico_sdk_import.cmake)

project(pico-transmitter C CXX ASM)
set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)


pico_sdk_init()

add_executable(transmitter
	transmitter.cpp
	LoRa-RP2040.cpp
	Print.cpp
	)


target_link_libraries(transmitter pico_stdlib hardware_spi hardware_uart)

pico_enable_stdio_usb(transmitter 1)
pico_enable_stdio_uart(transmitter 1)

pico_add_extra_outputs(transmitter)
