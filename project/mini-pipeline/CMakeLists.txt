cmake_minimum_required(VERSION 3.14)
project(mini_pipeline CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 消毒剂：cmake -DMP_SANITIZE=address,undefined 或 -DMP_SANITIZE=thread
set(MP_SANITIZE "" CACHE STRING "Sanitizer: address,undefined | thread | 空")
if(MP_SANITIZE)
  add_compile_options(-fsanitize=${MP_SANITIZE} -fno-omit-frame-pointer -g)
  add_link_options(-fsanitize=${MP_SANITIZE})
  message(STATUS "mini-pipeline: sanitizers = ${MP_SANITIZE}")
endif()

find_package(Threads REQUIRED)

add_library(mini_pipeline src/buffer_pool.cpp)
target_include_directories(mini_pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mini_pipeline PUBLIC Threads::Threads)
target_compile_options(mini_pipeline PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall;-Wextra;-Wpedantic;-Wshadow>)

enable_testing()

# 复用仓库共享的 gtest（third_party 里用 FetchContent 声明）
file(GLOB MP_TESTS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.cpp)
if(MP_TESTS)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../third_party ${CMAKE_BINARY_DIR}/third_party)
  foreach(test_src ${MP_TESTS})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE mini_pipeline gtest_main gtest)
    add_test(NAME ${test_name} COMMAND ${test_name})
  endforeach()
else()
  message(STATUS "mini-pipeline: tests/ 下暂无 *_test.cpp")
endif()
