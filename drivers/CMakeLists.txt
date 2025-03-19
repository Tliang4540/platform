cmake_minimum_required(VERSION 3.20)

add_library(driverslib)

CONFIGURE_FILE(include/bsp_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/bsp_config.h)

if(${CHIP} MATCHES "STM32")
    aux_source_directory(stm32 DRIVERS_SRC)
endif()

if (BSP_USING_SPI_OLED)
    set(DRIVERS_SRC ${DRIVERS_SRC} oled/oled_spi.c)
endif()
if (BSP_USING_SPIFLASH)
    set(DRIVERS_SRC ${DRIVERS_SRC} spiflash/spiflash.c)
endif()

target_sources(driverslib PRIVATE ${DRIVERS_SRC})
target_include_directories(driverslib PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(driverslib PRIVATE archlib componentslib platform)

target_include_directories(driverslib PUBLIC include)
