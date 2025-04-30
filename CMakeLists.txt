cmake_minimum_required(VERSION 3.13)

project(rene)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard


file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)
add_executable(rene ${SOURCE_FILES})

#
# my ut library
#

include(FetchContent)
FetchContent_Declare(
        ut
        GIT_REPOSITORY https://github.com/jmitchell24/ut.git)
FetchContent_MakeAvailable(ut)
target_link_libraries(rene PRIVATE ut)

# FTXUI
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG v6.1.1
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

target_link_libraries(rene PRIVATE ftxui::dom ftxui::screen ftxui::component)