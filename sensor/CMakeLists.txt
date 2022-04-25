
# Find all source files in a single current directory
# Save the name to DIR_Config_SRCS
aux_source_directory(. TEST)

# Generate the link library
add_library(bme680 ${TEST})
target_link_libraries(bme680 PUBLIC pico_stdlib hardware_i2c)