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
    src/game_menu.cpp
    src/crash_handler.cpp
    src/package_setup.cpp
)

if(WIN32)
    add_executable(poker_smash WIN32 ${POKERSMASH_SOURCES})
else()
    add_executable(poker_smash ${POKERSMASH_SOURCES})
endif()

rexglue_setup_target(poker_smash GPU_PLUGINS xenos)

# Linker map: turns crash-report offsets (poker_smash.exe+0x...) into
# recompiled function names (sub_82xxxxxx = guest address).
if(WIN32)
    target_link_options(poker_smash PRIVATE "LINKER:/MAP:${CMAKE_BINARY_DIR}/poker_smash.map")
endif()
