cmake_minimum_required(VERSION 3.16)
project(pi_rgb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(pi_rgb src/main.cpp)
target_compile_options(pi_rgb PRIVATE -Wall -Wextra)

option(ON_RPI "Build with RGB matrix support" OFF)

if(ON_RPI)
    set(RGB_DIR ${CMAKE_SOURCE_DIR}/third_party/rpi-rgb-led-matrix)
    set(RGB_LIB ${RGB_DIR}/lib/librgbmatrix.a)

    add_custom_command(
        OUTPUT ${RGB_LIB}
        COMMAND make -C ${RGB_DIR}/lib
        COMMENT "Building rpi-rgb-led-matrix"
    )
    add_custom_target(rgbmatrix_lib DEPENDS ${RGB_LIB})

    add_dependencies(pi_rgb rgbmatrix_lib)
    target_include_directories(pi_rgb PRIVATE ${RGB_DIR}/include)
    target_link_libraries(pi_rgb PRIVATE ${RGB_LIB} rt m pthread)
    target_compile_definitions(pi_rgb PRIVATE ON_RPI=1)
endif()