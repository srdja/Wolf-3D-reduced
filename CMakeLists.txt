cmake_minimum_required (VERSION 3.0)
project                (WOLF3D_REDUX)

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

add_subdirectory       (src)
