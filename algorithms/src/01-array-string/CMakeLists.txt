cmake_minimum_required(VERSION 3.14)
project(array_string)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
# 共享第三方依赖（gtest）
# ============================================================
add_subdirectory(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party
  ${CMAKE_BINARY_DIR}/third_party
)

enable_testing()

# ============================================================
# 编译参数（可在命令行指定）
# ============================================================
# BUILD_TEST_TARGET — 只编译指定的测试目标，空表示编译全部
#   示例: cmake -DBUILD_TEST_TARGET=two_sum_test ..
# TEST_FILTER      — gtest 过滤模式，只运行匹配的用例
#   示例: cmake -DTEST_FILTER="TwoSumTest.*" ..
set(BUILD_TEST_TARGET "" CACHE STRING "Build only this test target (empty = all)")
set(TEST_FILTER "*" CACHE STRING "gtest filter pattern, e.g. 'TwoSumTest.*'")

# ============================================================
# 辅助函数：注册一个测试目标（受 BUILD_TEST_TARGET 控制）
# ============================================================
function(add_single_test name src)
  if(NOT BUILD_TEST_TARGET OR BUILD_TEST_TARGET STREQUAL name)
    add_executable(${name} ${src})
    target_link_libraries(${name} PRIVATE gtest_main gtest)
    add_test(NAME ${name} COMMAND ${name} --gtest_filter=${TEST_FILTER})
  endif()
endfunction()

# ============================================================
# 测试用例 — 每题独立可执行
# ============================================================

# 双指针 (每题一个测试源文件)
add_single_test(two_sum_test tests/two_sum_test.cpp)
add_single_test(max_area_test tests/max_area_test.cpp)
add_single_test(remove_duplicates_test tests/remove_duplicates_test.cpp)
add_single_test(remove_element_test tests/remove_element_test.cpp)
add_single_test(move_zeroes_test tests/move_zeroes_test.cpp)
add_single_test(three_sum_test tests/three_sum_test.cpp)

# 滑动窗口
add_single_test(sliding_window_test tests/02_sliding_window_test.cpp)

# 前缀和
add_single_test(prefix_sum_test tests/03_prefix_sum_test.cpp)

# 字符串高频技巧
add_single_test(string_basics_test tests/05_string_basics_test.cpp)
