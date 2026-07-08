cmake_minimum_required(VERSION 3.18)
project(doom)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Usa SOLO doom_jni_safe.c para probar
add_library(doom SHARED
    doom_jni_safe.c
)

target_include_directories(doom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(doom
    log
    m
)

target_compile_options(doom PRIVATE -O2)