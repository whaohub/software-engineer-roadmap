cmake_minimum_required(VERSION 3.16)
project(order_relaxed LANGUAGES CXX)

# ---- 语言标准 ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- 默认构建类型 ----
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ---- 线程库（必须）----
find_package(Threads REQUIRED)

# ---- 通用告警与调试参数 ----
add_library(project_flags INTERFACE)
target_compile_options(project_flags INTERFACE
  -Wall
  -Wextra
  # 可选增强，按需打开：
  # -Wpedantic
  # -Wshadow
  # -Wconversion
  # -Werror
)
target_compile_options(project_flags INTERFACE
  $<$<CONFIG:Debug>:-O0;-g3>
)
target_compile_options(project_flags INTERFACE
  $<$<CONFIG:Release>:-O2>
)

# ---- 可执行目标 ----
add_executable( demo_atomic
  demo_atomic.cpp
)
target_link_libraries(demo_atomic PRIVATE project_flags Threads::Threads)

# ---- Sanitizer 开关（一次只能开一种）----
option(USE_TSAN "Build with ThreadSanitizer" ON)
option(USE_ASAN "Build with Address+UB Sanitizer" OFF)

if(USE_TSAN AND USE_ASAN)
  message(FATAL_ERROR "USE_TSAN 与 USE_ASAN 不能同时开启")
endif()

if(USE_TSAN)
  target_compile_options(demo_atomic PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
  target_link_options(demo_atomic PRIVATE -fsanitize=thread)
endif()

if(USE_ASAN)
  target_compile_options(demo_atomic PRIVATE
    -fsanitize=address,undefined
    -fno-omit-frame-pointer
    -fno-sanitize-recover=all
  )
  target_link_options(demo_atomic PRIVATE -fsanitize=address,undefined)
endif()