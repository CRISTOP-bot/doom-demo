cmake_minimum_required(VERSION 3.18)
project(doom C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Incluye TODOS los archivos fuente
add_library(doom SHARED
    doom_jni.c
    game_logic.c
    opengl_es_render.c
)

target_include_directories(doom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(doom
    log
    GLESv2
    android
    m
)

target_compile_options(doom PRIVATE -O2)