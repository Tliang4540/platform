cmake_minimum_required(VERSION 3.22)

add_library(componentslib
    ringbuffer/ringbuffer.c
    mylib/mylib.c
    graphic/graphic.c
)

target_include_directories(componentslib PUBLIC
    ringbuffer
    mylib
    graphic
)
