cmake_minimum_required(VERSION 3.14)
project(mini_shell C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(mini_shell mini_shell.c)
target_compile_options(mini_shell PRIVATE -Wall -Wextra)
