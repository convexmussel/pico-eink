
# Find all source files in a single current directory
# Save the name to DIR_Config_SRCS
aux_source_directory(. TEST)

# Generate the link library
add_library(einkt ${TEST})
target_link_libraries(einkt PUBLIC pico_stdlib Fonts Config)