# arm_toolchain.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Compilateur
set(CMAKE_C_COMPILER "D:/Documents/M2/RTOS/zephyr-sdk-0.17.4/arm-zephyr-eabi/bin/arm-zephyr-eabi-gcc.exe")
set(CMAKE_CXX_COMPILER "D:/Documents/M2/RTOS/zephyr-sdk-0.17.4/arm-zephyr-eabi/bin/arm-zephyr-eabi-g++.exe")
set(CMAKE_ASM_COMPILER "D:/Documents/M2/RTOS/zephyr-sdk-0.17.4/arm-zephyr-eabi/bin/arm-zephyr-eabi-gcc.exe")

# Ne pas chercher des programmes dans le système hôte
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Dire à CMake que le compilateur fonctionne
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# Forcer le type de target à bibliothèque statique (pas d'exe Windows)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)