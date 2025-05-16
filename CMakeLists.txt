cmake_minimum_required(VERSION 3.13)



project(
    rené
    VERSION "25.5.16"
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


set(PROJECT_RELEASE "pre-release")
set(PROJECT_BINARY "rene")

# Enable the C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
# my ut library
#

include(FetchContent)
FetchContent_Declare(
        ut
        GIT_REPOSITORY https://github.com/jmitchell24/ut.git)
FetchContent_MakeAvailable(ut)
target_link_libraries(${PROJECT_BINARY} PRIVATE ut)

# FTXUI
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG v6.1.1
)

FetchContent_GetProperties(ftxui)
FetchContent_MakeAvailable(ftxui)
target_link_libraries(${PROJECT_BINARY} PRIVATE ftxui::dom ftxui::screen ftxui::component)
