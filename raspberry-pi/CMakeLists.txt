cmake_minimum_required(VERSION 3.13)

# Use C99 standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Board configuration - must match your hardware
set(PICO_BOARD pico2_w)

# Fetch Pico SDK if not already present
set(PICO_SDK_FETCH_FROM_GIT ON)
include(pico_sdk_import.cmake)

# Project name
project(Raspberry-Pi-Water-irrigation-System C)

# Initialize the SDK
pico_sdk_init()

# Add executable and source files
add_executable(Raspberry-Pi-Water-irrigation-System
    src/main.c
    src/soil_sensor.c
    src/pump_control.c
    # Add more .c files as needed
)

# Include directories (e.g., for header files)
target_include_directories(Raspberry-Pi-Water-irrigation-System PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Link required libraries
target_link_libraries(Raspberry-Pi-Water-irrigation-System
    pico_stdlib
    hardware_adc
    hardware_pwm
    hardware_i2c
    hardware_spi
    pico_cyw43_arch_lwip_threadsafe_background
)

# Enable serial output (choose USB or UART)
pico_enable_stdio_usb(Raspberry-Pi-Water-irrigation-System 1)
pico_enable_stdio_uart(Raspberry-Pi-Water-irrigation-System 0)

# Generate additional outputs (.uf2, .bin, .hex)
pico_add_extra_outputs(Raspberry-Pi-Water-irrigation-System)
