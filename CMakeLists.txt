# poker_smash - ReXGlue Recompiled Project
#
# SDK boilerplate lives in generated/rexglue.cmake.

cmake_minimum_required(VERSION 3.25)
project(poker_smash LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(generated/rexglue.cmake)

# Sources
set(POKERSMASH_SOURCES
    src/main.cpp
    src/usbcam_stubs.cpp
)

if(WIN32)
    add_executable(poker_smash WIN32 ${POKERSMASH_SOURCES})
else()
    add_executable(poker_smash ${POKERSMASH_SOURCES})
endif()

rexglue_setup_target(poker_smash GPU_PLUGINS xenos)
