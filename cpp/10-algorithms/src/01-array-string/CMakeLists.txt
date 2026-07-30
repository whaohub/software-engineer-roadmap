cmake_minimum_required(VERSION 3.14)
project(array_string)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# 共享第三方依赖（gtest 等）
# 实际下载仅在首次构建时发生一次，缓存于 build/_deps
# ============================================================
add_subdirectory(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party
  ${CMAKE_BINARY_DIR}/third_party
)

enable_testing()

# ============================================================
# 双指针 — 带 gtest 的测试版
# ============================================================
add_executable(two_pointers_test 01_two_pointers.cpp)
target_link_libraries(two_pointers_test PRIVATE gtest_main gtest)
target_compile_definitions(two_pointers_test PRIVATE USE_GTEST)

add_test(NAME two_pointers_test COMMAND two_pointers_test)

# 滑动窗口
# add_executable(sliding_window 02_sliding_window.cpp)

# # 前缀和
# add_executable(prefix_sum 03_prefix_sum.cpp)

# # 矩阵模拟
# add_executable(matrix_simulation 04_matrix_simulation.cpp)

# # 字符串基础
# add_executable(string_basics 05_string_basics.cpp)

# # 字符串进阶
# add_executable(string_advanced 06_string_advanced.cpp)

# # 其他高频题
# add_executable(lc_top_hot 07_lc_top_hot.cpp)
