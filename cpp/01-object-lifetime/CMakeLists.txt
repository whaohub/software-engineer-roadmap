cmake_minimum_required(VERSION 3.10)
project(ObjectLifetime)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(mini_unique_ptr src/mini_unique_ptr.cpp)
add_executable(test_exception src/test_exception.cpp)
