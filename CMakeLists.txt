cmake_minimum_required(VERSION 3.16)
project(pi_rgb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

add_executable(pi_rgb src/main.cpp)
target_include_directories(pi_rgb PRIVATE include)
target_compile_options(pi_rgb PRIVATE -Wall -Wextra)