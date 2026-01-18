cmake_minimum_required(VERSION 3.13)

# Initialize Pico SDK (must be before project())
set(PICO_PLATFORM rp2350)
set(PICO_BOARD pico2)

include(pico_sdk_import.cmake)

project(superpico-digital C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Add PicoHDMI library
add_subdirectory(lib/pico_hdmi)

# Main executable
add_executable(superpico-digital
    src/main.c
)

target_link_libraries(superpico-digital
    pico_stdlib
    pico_multicore
    pico_hdmi
)

# Create UF2 output
pico_add_extra_outputs(superpico-digital)

# Enable USB output, disable UART
pico_enable_stdio_usb(superpico-digital 1)
pico_enable_stdio_uart(superpico-digital 0)
