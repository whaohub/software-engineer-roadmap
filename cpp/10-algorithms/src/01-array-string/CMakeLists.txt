cmake_minimum_required(VERSION 3.10)
project(array_string)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 双指针
add_executable(two_pointers 01_two_pointers.cpp)

# 滑动窗口
add_executable(sliding_window 02_sliding_window.cpp)

# 前缀和
add_executable(prefix_sum 03_prefix_sum.cpp)

# 矩阵模拟
add_executable(matrix_simulation 04_matrix_simulation.cpp)

# 字符串基础
add_executable(string_basics 05_string_basics.cpp)

# 字符串进阶
add_executable(string_advanced 06_string_advanced.cpp)

# 其他高频题
add_executable(lc_top_hot 07_lc_top_hot.cpp)
