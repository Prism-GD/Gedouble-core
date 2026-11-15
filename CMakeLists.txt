cmake_minimum_required(VERSION 3.22.1)
project(gedouble_core LANGUAGES CXX)

add_library(gedouble SHARED
    native/src/gedouble_core.cpp
    native/src/module_manager.cpp
)

target_include_directories(gedouble
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/native/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/native/src
)

target_compile_features(gedouble PUBLIC cxx_std_20)
target_compile_options(gedouble PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -fvisibility=hidden
)

find_library(android_log_lib log)
if(android_log_lib)
    target_link_libraries(gedouble PRIVATE ${android_log_lib})
endif()
