cmake_minimum_required(VERSION 3.20)

if (NOT DEFINED PROJECT_NAME)
    set(PLATFORM_PATH .)
    include(cmake/bsp_template.cmake)
    project(platform LANGUAGES C ASM)
endif()

add_library(platform src/device.c src/log.c src/tinyos.c)
set_source_files_properties(src/tinyos.c PROPERTIES COMPILE_DEFINITIONS "OS_TASK_NUM_MAX=${OS_TASK_NUM_MAX};OS_GET_FREE_STACK_ENABLE=${OS_GET_FREE_STACK_ENABLE}")

add_subdirectory(arch)
add_subdirectory(components)
add_subdirectory(drivers)

target_link_libraries(platform PRIVATE archlib)
target_link_libraries(platform PUBLIC driverslib componentslib)

target_include_directories(platform PUBLIC include)

target_compile_definitions(platform PUBLIC LOG_LEVEL=${LOG_LEVEL})
