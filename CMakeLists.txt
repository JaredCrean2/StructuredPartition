cmake_minimum_required(VERSION 3.12.0)

# set project name
project(StructuredPartition)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)


set(ALL_LIBS "")

add_subdirectory(src)
add_subdirectory(test)
