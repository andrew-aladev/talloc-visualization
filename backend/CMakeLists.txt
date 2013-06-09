cmake_minimum_required (VERSION 2.8)
project (talloc_visualization)

set (CMAKE_BUILD_TYPE Debug)

set (WSLAY_TARGET talloc2)
add_subdirectory (talloc2)
include_directories ("talloc2/src/")

set (WSLAY_TARGET wslay)
add_subdirectory (wslay)
include_directories ("wslay/src/")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Winline -std=gnu99")
set (CMAKE_C_FLAGS_DEBUG "-O0 -g")

add_subdirectory (src)

include_directories ("src/")

add_subdirectory (examples)