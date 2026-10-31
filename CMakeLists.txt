cmake_minimum_required(VERSION 3.31)

project(echo_server
    VERSION 0.0.0
    DESCRIPTION "Platform backend for ECHO"
    LANGUAGES CXX)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "不要在源码目录里构建，请用 cmake --preset <debug|release|asan>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(ECHO_SANITIZE "用 AddressSanitizer 和 UndefinedBehaviorSanitizer 编译" OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

if(ECHO_SANITIZE)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer
        -fno-sanitize-recover=all)
    add_link_options(-fsanitize=address,undefined)
endif()

find_package(Drogon CONFIG REQUIRED)

option(BUILD_TESTING "构建测试" ON)
if(BUILD_TESTING)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)
    include(GoogleTest)
endif()

add_subdirectory(apps/platform)
