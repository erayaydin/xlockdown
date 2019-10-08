# Cmake Options
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Register general cmake commands
include(cmake/General.cmake)

# Set policies for xlockdown project
set_policy(CMP0054 NEW) # ENABLE CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
set_policy(CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.
set_policy(CMP0063 NEW) # ENABLE CMP0063: Honor visibility properties for all target types.
set_policy(CMP0077 NEW) # ENABLE CMP0077: option() honors normal variables

# Set project
project(xlockdown)

# Add executable
add_executable(xlockdown main.cpp)

# X11 Library
find_package(X11)
if(X11_FOUND)
    include_directories(${X11_INCLUDE_DIRS})
    target_link_libraries(xlockdown ${X11_LIBRARIES})
endif(X11_FOUND)

# Crypt
target_link_libraries(xlockdown crypt)

target_link_libraries(xlockdown pam)
target_link_libraries(xlockdown pam_misc)