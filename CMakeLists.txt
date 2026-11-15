cmake_minimum_required(VERSION 3.22.1)
project(gedouble_core LANGUAGES CXX)

add_library(gedouble SHARED
    native/src/gedouble_core.cpp
)

target_include_directories(gedouble
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/native/include
)

target_compile_features(gedouble PRIVATE cxx_std_20)
target_compile_options(gedouble PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -fvisibility=hidden
)
