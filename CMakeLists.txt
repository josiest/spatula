cmake_minimum_required(VERSION 3.10)

project(kdtree VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(kdtree INTERFACE)
target_include_directories(kdtree INTERFACE include)