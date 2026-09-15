cmake_minimum_required(VERSION 3.14)
project(linked_list)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
# 编译参数（可在命令行指定）
# ============================================================
# BUILD_TEST_TARGET — 只编译指定的测试目标，空表示编译全部
#   示例: cmake -DBUILD_TEST_TARGET=reverse_list_test ..
# TEST_FILTER      — gtest 过滤模式，只运行匹配的用例
#   示例: cmake -DTEST_FILTER="ReverseListTest.*" ..
set(BUILD_TEST_TARGET "" CACHE STRING "Build only this test target (empty = all)")
set(TEST_FILTER "*" CACHE STRING "gtest filter pattern, e.g. 'ReverseListTest.*'")

# ============================================================
# 骨架源文件 —— 每个文件对应一种题目模式
# ============================================================
set(LINKED_LIST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/01_dummy_head.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/02_two_pointers.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/03_reverse.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/04_merge_sort.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/05_design.cpp
)

# 语法校验目标：OBJECT 库不做链接，因此即使函数只有 TODO 空实现、
# 也还没有任何测试，`cmake --build` 依然会把全部骨架编译一遍。
add_library(linked_list_skeletons OBJECT ${LINKED_LIST_SOURCES})
target_include_directories(linked_list_skeletons PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(linked_list_skeletons PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall;-Wextra>
  # 骨架阶段 05_design.cpp 的类成员只用于说明数据结构布局、方法体仍是 TODO，
  # 该警告此时没有参考价值（-Wunused-parameter 仍需保留，用于校验 (void) 转换）
  $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wno-unused-private-field>)

enable_testing()

# ============================================================
# 辅助函数：注册一个测试目标
# ------------------------------------------------------------
# 与 01-array-string 的版本相比多加了一层 EXISTS 守卫：
# 列出了但尚未创建测试源文件时只跳过，不会让整个 configure 失败。
# ============================================================
function(add_single_test name src)
  if(NOT BUILD_TEST_TARGET OR BUILD_TEST_TARGET STREQUAL name)
    if(EXISTS ${src})
      add_executable(${name} ${src})
      target_link_libraries(${name} PRIVATE gtest_main gtest)
      target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
      add_test(NAME ${name} COMMAND ${name} --gtest_filter=${TEST_FILTER})
    else()
      message(STATUS "02-linked-list: 跳过 ${name}（源文件不存在: ${src}）")
    endif()
  endif()
endfunction()

# ============================================================
# 测试用例 —— 自动扫描 tests/*_test.cpp
# ------------------------------------------------------------
# 命名约定: tests/<target>_test.cpp → 可执行目标 <target>_test
# 新增测试文件后重新 configure 即自动生效，无需改本文件。
#
# 如需手工注册（覆盖自动扫描），按下面格式逐行添加即可：
#   add_single_test(reverse_list_test tests/reverse_list_test.cpp)
# ============================================================
file(GLOB LINKED_LIST_TESTS CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.cpp)

if(LINKED_LIST_TESTS)
  # gtest 仅在确实存在测试时才拉取，骨架阶段可离线构建
  add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party
    ${CMAKE_BINARY_DIR}/third_party
  )

  foreach(test_src ${LINKED_LIST_TESTS})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_single_test(${test_name} ${test_src})
  endforeach()
else()
  message(STATUS "02-linked-list: tests/ 下暂无 *_test.cpp，仅构建骨架语法校验目标")
endif()
