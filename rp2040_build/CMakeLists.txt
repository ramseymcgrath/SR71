# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialize pico_sdk
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(SR71-SPI C CXX ASM)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Add all source files
add_executable(SR71-SPI 
    SR71-SPI.cpp
    adafruit_max3421e.cpp
    usb_descriptors.cpp
    max3241e_hcd.cpp
)

pico_set_program_name(SR71-SPI "SR71-SPI")
pico_set_program_version(SR71-SPI "0.1")

# TinyUSB Configuration
target_compile_definitions(${PROJECT_NAME} PRIVATE
    CFG_TUSB_MCU=OPT_MCU_RP2040
    CFG_TUSB_OS=OPT_OS_PICO
    CFG_TUSB_RHPORT1_MODE=OPT_MODE_HOST
    CFG_TUSB_RHPORT0_MODE=OPT_MODE_DEVICE
    CFG_TUD_ENABLED=1      # Enable device stack
    CFG_TUH_ENABLED=1      # Enable host stack
    CFG_TUH_MAX3421=1      # Enable MAX3421 host controller
    CFG_TUH_DEBUG=0       # Debug level for host stack  
    CFG_TUD_HID=1
    CFG_TUD_CDC=0
    CFG_TUD_MSC=0
)

# Required Libraries
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_spi
    tinyusb_device
    tinyusb_host
    tinyusb_board            # Required for board-specific implementations
)

# Additional include paths if needed
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PICO_SDK_PATH}/lib/tinyusb/src
    ${PICO_SDK_PATH}/lib/tinyusb/hw/bsp/rp2040
    ${PICO_SDK_PATH}/src/portable/analog/max3421/hcd_max3421.c
)


# Generate additional output formats (uf2, hex, etc.)
pico_add_extra_outputs(${PROJECT_NAME})