# FORCE CMake pour cross-compilation ARM
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# D?sactiver COMPL?TEMENT les tests
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_ASM_COMPILER_WORKS TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Compilateurs
set(CMAKE_C_COMPILER "D:/Documents/M2/RTOS/zephyr-sdk-0.17.4/arm-zephyr-eabi/bin/arm-zephyr-eabi-gcc.exe")
set(CMAKE_CXX_COMPILER "D:/Documents/M2/RTOS/zephyr-sdk-0.17.4/arm-zephyr-eabi/bin/arm-zephyr-eabi-g++.exe")
set(CMAKE_ASM_COMPILER "D:/Documents/M2/RTOS/zephyr-sdk-0.17.4/arm-zephyr-eabi/bin/arm-zephyr-eabi-gcc.exe")

# Toolchain Zephyr
set(ZEPHYR_TOOLCHAIN_VARIANT zephyr)
set(ZEPHYR_SDK_INSTALL_DIR "D:/Documents/M2/RTOS/zephyr-sdk-0.17.4")

# Emp?cher CMake de chercher des librairies Windows
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
