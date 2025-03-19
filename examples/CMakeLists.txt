cmake_minimum_required(VERSION 3.20)

set(PLATFORM_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..) 

project(platform_test LANGUAGES C ASM)

add_subdirectory(stm32g070)
# add_subdirectory(stm32l051)
