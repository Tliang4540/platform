# set the chip
# set(CHIP STM32G070xx)
set(CHIP STM32L051xx)

# set vector table offset
set(VECT_TAB_OFFSET 0x0000)
# set log level
set(LOG_LEVEL 3)

# set task max num
set(OS_TASK_NUM_MAX 8)

# 使用SPI OLED
option(BSP_USING_SPI_OLED "Enable SPI OLED" ON)
# 使用SPIFLASH
option(BSP_USING_SPIFLASH "Enable SPIFLASH" ON)

# UART
set(BSP_UART_BUFFER_SIZE 128) #设置UART默认缓存大小，未单独设置时使用此值
set(BSP_UART1_TX_BUFSIZE 128) #单独设置UART1的发送缓存大小
set(BSP_UART1_RX_BUFSIZE 128) #单独设置UART1的接收缓存大小

option(BSP_USING_UART1 "Enable UART1" ON)
option(BSP_USING_UART2 "Enable UART2" ON)
option(BSP_USING_UART3 "Enable UART3" ON)
option(BSP_USING_UART4 "Enable UART4" ON)
option(BSP_USING_UART5 "Enable UART5" OFF)
option(BSP_USING_UART6 "Enable UART6" OFF)
option(BSP_USING_UART7 "Enable UART5" OFF)
option(BSP_USING_UART8 "Enable UART6" OFF)
option(BSP_USING_LPUART1 "Enable LPUART1" OFF)
option(BSP_USING_LPUART2 "Enable LPUART2" OFF)

# SPI
option(BSP_USING_SPI1 "Enable SPI1" ON)
option(BSP_SPI1_RX_USING_DMA "Enable SPI1 RX DMA" ON)
option(BSP_SPI1_TX_USING_DMA "Enable SPI1 TX DMA" ON)
option(BSP_USING_SPI2 "Enable SPI2" ON)
option(BSP_SPI2_RX_USING_DMA "Enable SPI2 RX DMA" OFF)
option(BSP_SPI2_TX_USING_DMA "Enable SPI2 TX DMA" OFF)
option(BSP_USING_SPI3 "Enable SPI3" OFF)
option(BSP_SPI3_RX_USING_DMA "Enable SPI3 RX DMA" OFF)
option(BSP_SPI3_TX_USING_DMA "Enable SPI3 TX DMA" OFF)

# I2C
option(BSP_USING_I2C1 "Enable I2C1" ON)
option(BSP_USING_I2C2 "Enable I2C2" ON)
option(BSP_USING_I2C3 "Enable I2C3" OFF)

# ADC
option(BSP_USING_ADC1 "Enable ADC1" ON)

# TIMER
option(BSP_USING_TIMER1 "Enable TIMER1" OFF)
option(BSP_USING_TIMER2 "Enable TIMER2" OFF)
option(BSP_USING_TIMER3 "Enable TIMER3" OFF)
option(BSP_USING_TIMER4 "Enable TIMER4" OFF)
option(BSP_USING_TIMER6 "Enable TIMER6" ON)
option(BSP_USING_TIMER7 "Enable TIMER7" OFF)
option(BSP_USING_TIMER14 "Enable TIMER14" OFF)
option(BSP_USING_TIMER15 "Enable TIMER15" OFF)
option(BSP_USING_TIMER16 "Enable TIMER16" OFF)
option(BSP_USING_TIMER17 "Enable TIMER17" OFF)
option(BSP_USING_TIMER21 "Enable TIMER21" OFF)
option(BSP_USING_TIMER22 "Enable TIMER22" OFF)

# PWM
option(BSP_USING_PWM1 "Enable PWM1" OFF)
option(BSP_USING_PWM2 "Enable PWM2" ON)
option(BSP_USING_PWM3 "Enable PWM3" OFF)
option(BSP_USING_PWM4 "Enable PWM4" OFF)
option(BSP_USING_PWM14 "Enable PWM14" OFF)
option(BSP_USING_PWM15 "Enable PWM15" OFF)
option(BSP_USING_PWM16 "Enable PWM16" OFF)
option(BSP_USING_PWM17 "Enable PWM17" OFF)
option(BSP_USING_PWM21 "Enable PWM21" OFF)
option(BSP_USING_PWM22 "Enable PWM22" OFF)

# WDT
option(BSP_USING_WDT "Enable WDT" ON)

# RTC
option(BSP_USING_RTC "Enable RTC" ON)

# AUDIO
option(BSP_USING_PWM_AUDIO "Enable PWM AUDIO" ON)
option(BSP_USING_DAC_AUDIO "Enable PWM AUDIO" OFF) # 未支持
if (BSP_USING_PWM_AUDIO OR BSP_USING_DAC_AUDIO)
    # 使用TIMER6或7触发DMA传输, 选择一个
    option(AUDIO_USING_TIMER6 "Enable TIMER6" ON)
    option(AUDIO_USING_TIMER7 "Enable TIMER7" OFF)
endif()

# set the toolchain prefix and target flags
set(TOOLCHAIN_PREFIX arm-none-eabi-)
set(TARGET_FLAGS "-mcpu=cortex-m0plus ")
include(${PLATFORM_PATH}/cmake/arm_toolchain.cmake)
