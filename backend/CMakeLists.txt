cmake_minimum_required(VERSION 2.8)
project(talloc_visualization)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(onion)
include_directories("onion/src/")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=gnu99")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g")
add_subdirectory(src)

include_directories("src/")

add_subdirectory(examples)