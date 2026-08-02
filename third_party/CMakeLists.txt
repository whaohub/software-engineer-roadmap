# ============================================================
# 第三方依赖管理
# 所有模块通过 add_subdirectory 引用此文件即可共享 gtest
# ============================================================

cmake_minimum_required(VERSION 3.14)
project(third_party)

# -----------------------------------------------------------
# Google Test
# -----------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
