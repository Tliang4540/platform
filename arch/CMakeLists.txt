cmake_minimum_required(VERSION 3.20)

add_library(archlib)

string(TOLOWER ${CHIP} CHIP_LPWER)
if(${CHIP} MATCHES "STM32G0")
    target_compile_definitions(archlib PUBLIC USE_FULL_LL_DRIVER USER_VECT_TAB_ADDRESS VECT_TAB_OFFSET=${VECT_TAB_OFFSET} ${CHIP} STM32G0)
    aux_source_directory(arm/stm32/stm32g0xx           SYSTEM_SRC)
    aux_source_directory(arm/stm32/stm32g0xx/lib/Src   LIB_LL_SRC)

    set(ARCH_SRC ${SYSTEM_SRC} ${LIB_LL_SRC} arm/stm32/stm32g0xx/startup/startup_${CHIP_LPWER}.s arm/cortex-m0/context.s arm/cortex-m0/cpuport.c)
    set(ARCH_INC arm/include arm/stm32/include arm/stm32/stm32g0xx/include arm/stm32/stm32g0xx/lib/Inc)
elseif(${CHIP} MATCHES "STM32L0")
    target_compile_definitions(archlib PUBLIC USE_FULL_LL_DRIVER USER_VECT_TAB_ADDRESS VECT_TAB_OFFSET=${VECT_TAB_OFFSET} ${CHIP} STM32L0)
    aux_source_directory(arm/stm32/stm32l0xx           SYSTEM_SRC)
    aux_source_directory(arm/stm32/stm32l0xx/lib/Src   LIB_LL_SRC)

    set(ARCH_SRC ${SYSTEM_SRC} ${LIB_LL_SRC} arm/stm32/stm32l0xx/startup/startup_${CHIP_LPWER}.s arm/cortex-m0/context.s arm/cortex-m0/cpuport.c)
    set(ARCH_INC arm/include arm/stm32/include arm/stm32/stm32l0xx/include arm/stm32/stm32l0xx/lib/Inc)
else()
    message(FATAL_ERROR "ARCH ${ARCH} Undefined.")
endif()

target_sources(archlib PRIVATE ${ARCH_SRC})

target_include_directories(archlib PUBLIC ${ARCH_INC})
