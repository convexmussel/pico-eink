# Find all source files in a single current directory
# Save the name to DIR_ePaper_SRCS
aux_source_directory(. display)

include_directories(../lib/Config)
include_directories(../lib/GUI)
include_directories(../lib/e-Paper)
# Generate the link library
add_library(display ${display})
target_link_libraries(display PUBLIC Config ePaper GUI Fonts Config )