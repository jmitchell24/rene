cmake_minimum_required(VERSION 3.13)

project(rene)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(rene ${SOURCE_FILES}
        src/cmd.hpp
        src/cmd.cpp)

target_link_libraries(rene PRIVATE Threads::Threads ZLIB::ZLIB)

#
# my ut library
#

include(FetchContent)
FetchContent_Declare(
        ut
        GIT_REPOSITORY https://github.com/jmitchell24/ut.git)
FetchContent_MakeAvailable(ut)
target_link_libraries(rene PRIVATE ut)
