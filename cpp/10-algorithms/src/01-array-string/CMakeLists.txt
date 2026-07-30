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
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../../cpp/third_party
  ${CMAKE_BINARY_DIR}/third_party
)

enable_testing()

# ============================================================
# 测试用例 — 每个模块独立可执行
# ============================================================

# 双指针
add_executable(two_pointers_test tests/01_two_pointers_test.cpp)
target_link_libraries(two_pointers_test PRIVATE gtest_main gtest)
add_test(NAME two_pointers_test COMMAND two_pointers_test)

# 滑动窗口
add_executable(sliding_window_test tests/02_sliding_window_test.cpp)
target_link_libraries(sliding_window_test PRIVATE gtest_main gtest)
add_test(NAME sliding_window_test COMMAND sliding_window_test)

# 前缀和
add_executable(prefix_sum_test tests/03_prefix_sum_test.cpp)
target_link_libraries(prefix_sum_test PRIVATE gtest_main gtest)
add_test(NAME prefix_sum_test COMMAND prefix_sum_test)

# 字符串高频技巧
add_executable(string_basics_test tests/05_string_basics_test.cpp)
target_link_libraries(string_basics_test PRIVATE gtest_main gtest)
add_test(NAME string_basics_test COMMAND string_basics_test)
