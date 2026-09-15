cmake_minimum_required(VERSION 3.14)
project(mini_shell C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# -----------------------------------------------------------
# apue 第三方依赖:linux/apue 作为 subdir 引入(libapue.a)
# -----------------------------------------------------------
set(APUE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../apue)
if(NOT TARGET apue)
  add_subdirectory(${APUE_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/apue)
endif()

# add_executable(mini_shell mini_shell.c)
add_executable(process_demo process_demo.cpp)
target_compile_options(process_demo PRIVATE -Wall -Wextra)
target_link_libraries(process_demo PRIVATE apue::apue)
