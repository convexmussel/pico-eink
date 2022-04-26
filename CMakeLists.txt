cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(Pico_ePaper_Code)
pico_sdk_init()

# add a compilation subdirectory
add_subdirectory(./einkt/Config)
add_subdirectory(lib/e-Paper)
add_subdirectory(lib/Fonts)
add_subdirectory(lib/GUI)
add_subdirectory(./einkt/Fonts)
add_subdirectory(./rtc)
add_subdirectory(./sensor)
add_subdirectory(examples)
add_subdirectory(./display)
add_subdirectory(./einkt)

# add a header directory
include_directories(./rtc)
include_directories(./einkt/Fonts)
include_directories(examples)
include_directories(./einkt/Config)
include_directories(./lib/GUI)
include_directories(./sensor)
include_directories(./display)
include_directories(./einkt)

# generate an executable file
add_executable(epd 
main.c
)

# enable usb output, disable uart output
pico_enable_stdio_usb(epd 0)
pico_enable_stdio_uart(epd 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(epd)

target_link_libraries(epd DS1307 pico_stdlib hardware_spi hardware_i2c bme680 einkt Fonts Config display FontsX ePaper Config GUI examples)