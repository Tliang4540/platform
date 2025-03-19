cmake_minimum_required(VERSION 3.20)

# set(PLATFORM_PATH ..)
set(LINK_LD_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# project(platform_test LANGUAGES C ASM)

include(bsp_stm32g0.cmake)

add_executable(${CMAKE_PROJECT_NAME} main.c led_dev/led_dev.c audio_test/audio_test.c)

add_subdirectory(${PLATFORM_PATH} platform)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE platform)

# Create Flash Image
add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    # COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
)

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} ARGS --source --all-headers --demangle --line-numbers --wide ${CMAKE_PROJECT_NAME}.elf > ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.lst
	#--all-headers --demangle --disassemble -M xw 
)