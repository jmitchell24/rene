cmake_minimum_required(VERSION 3.13)



project(
    rené
    VERSION "25.5.23"
    DESCRIPTION "a powerful & minimalist auto-namer."
)

# release names: 
# algoma
# wawa
# thessalon 
# espanola 
# sudbury 
# timmins
# test


set(PROJECT_RELEASE "test7")
set(PROJECT_BINARY "rene")

# Enable the C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Force Static Linking of all dependencies
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

# Size reduction (LTO, stripping unused sections)
# not for debug builds
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
endif()

# Configure headers

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rene.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rene.hpp"
)

file(GLOB_RECURSE SOURCE_FILES
        src/*.cpp
        src/*.hpp
)

add_executable(rene ${SOURCE_FILES})

#
# UT Code Library
#
include(FetchContent)
FetchContent_Declare(
        ut
        GIT_REPOSITORY https://github.com/jmitchell24/ut.git)
FetchContent_MakeAvailable(ut)
target_link_libraries(${PROJECT_BINARY} PRIVATE ut)

#
# FTXUI
#
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG v6.1.1
)

FetchContent_GetProperties(ftxui)
FetchContent_MakeAvailable(ftxui)
target_link_libraries(${PROJECT_BINARY} PRIVATE ftxui::dom ftxui::screen ftxui::component)

# Find UPX
find_program(UPX_EXECUTABLE upx)

# Run UPX if available (not for debug builds)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(UPX_EXECUTABLE)
        message(STATUS "Found UPX: ${UPX_EXECUTABLE}")

        # Add UPX compression as post-build step
        add_custom_command(TARGET ${PROJECT_BINARY} POST_BUILD
                COMMAND ${UPX_EXECUTABLE} --best $<TARGET_FILE:${PROJECT_BINARY}>
                COMMENT "Compressing binary with UPX"
                VERBATIM)
    else()
        message(WARNING "UPX not found - binary will not be compressed")
    endif()
endif()
