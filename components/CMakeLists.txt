cmake_minimum_required(VERSION 3.22)

add_library(componentslib
    ringbuffer/ringbuffer.c
    tinylib/tinylib.c
    graphic/graphic.c
    tinydb/tinydb.c
)

target_include_directories(componentslib PUBLIC
    ringbuffer
    tinylib
    graphic
    tinydb
)

target_link_libraries(componentslib PRIVATE driverslib platform)
