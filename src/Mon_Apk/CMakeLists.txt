cmake_minimum_required(VERSION 3.20.0)

# BYPASS COMPLET DES TESTS CMake
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE) 
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(blink)
target_sources(app PRIVATE src/main.c src/ble_uart.c   src/gps.c
src/lora.c)
