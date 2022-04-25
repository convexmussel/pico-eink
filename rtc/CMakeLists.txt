# Find all source files in a single current directory
# Save the name to DIR_Config_SRCS
aux_source_directory(. TEST)

# Generate the link library
add_library(DS1307 ${TEST})
target_link_libraries(DS1307 PUBLIC pico_stdlib hardware_i2c)