cmake_minimum_required(VERSION 3.15)
project(unit_type_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test_bin test.cpp)

target_include_directories(test_bin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(test_bin PRIVATE -Wall -Wextra -pedantic)
set_target_properties(test_bin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
