# ============================================================
# apue — UNIX 环境高级编程(APUE)第三方支持库
# 来源: https://github.com/ansontao/apue  (本地 clone 到 linux/apue)
#
# apue 仓库未提供 CMake 支持。此文件把 error.c 封装为静态库
# libapue.a(提供 err_sys / err_ret / set_fl 等辅助函数),
# apue.h 通过 PUBLIC include 目录传播给所有链接者。
# ============================================================

cmake_minimum_required(VERSION 3.14)
project(apue C)

add_library(apue STATIC error.c)
add_library(apue::apue ALIAS apue)

target_include_directories(apue PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(apue PUBLIC c_std_99)
