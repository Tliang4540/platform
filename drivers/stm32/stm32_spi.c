#include <spi.h>
#include <pin.h>
#include <stm32.h>
#include <bsp_config.h>
#include <tinyos.h>
#include <log.h>

#if defined(BSP_USING_SPI1) || defined(BSP_USING_SPI2) || defined(BSP_USING_SPI3)

#if defined(STM32G0)
#define __SPI1_DMA_IRQn           (10)
#define __SPI2_DMA_IRQn           (11)
#define __SPI3_DMA_IRQn           (11)

#define __SPI1_RX_DMA_CH          (LL_DMA_CHANNEL_2)
#define __SPI1_TX_DMA_CH          (LL_DMA_CHANNEL_3)
#define __SPI2_RX_DMA_CH          (LL_DMA_CHANNEL_4)
#define __SPI2_TX_DMA_CH          (LL_DMA_CHANNEL_5)
#define __SPI3_RX_DMA_CH          (LL_DMA_CHANNEL_6)
#define __SPI3_TX_DMA_CH          (LL_DMA_CHANNEL_7)

#define __SPI1_RX_DMA_REQ         (LL_DMAMUX_REQ_SPI1_RX)
#define __SPI1_TX_DMA_REQ         (LL_DMAMUX_REQ_SPI1_TX)
#define __SPI2_RX_DMA_REQ         (LL_DMAMUX_REQ_SPI2_RX)
#define __SPI2_TX_DMA_REQ         (LL_DMAMUX_REQ_SPI2_TX)
#define __SPI3_RX_DMA_REQ         (LL_DMAMUX_REQ_SPI3_RX)
#define __SPI3_TX_DMA_REQ         (LL_DMAMUX_REQ_SPI3_TX)

#define __SPI1_CLK_ENABLE()       (RCC->APBENR2 |= (1 << 12))
#define __SPI1_CLK_DISABLE()      (RCC->APBENR2 &= ~(1 << 12))
#define __SPI2_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 14))
#define __SPI2_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 14))
#define __SPI3_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 15))
#define __SPI3_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 15))
#elif defined (STM32L0) || defined (STM32F0)
#define __SPI1_DMA_IRQn           (10)
#define __SPI2_DMA_IRQn           (11)

#if defined (STM32L0)
#define __SPI1_RX_DMA_REQ         (LL_DMA_REQUEST_1)
#define __SPI1_TX_DMA_REQ         (LL_DMA_REQUEST_1)
#define __SPI2_RX_DMA_REQ         (LL_DMA_REQUEST_2)
#define __SPI2_TX_DMA_REQ         (LL_DMA_REQUEST_2)
#else
#define __SPI1_RX_DMA_REQ         (0)
#define __SPI1_TX_DMA_REQ         (0)
#define __SPI2_RX_DMA_REQ         (0)
#define __SPI2_TX_DMA_REQ         (0)
#endif

#define __SPI1_RX_DMA_CH          (LL_DMA_CHANNEL_2)
#define __SPI1_TX_DMA_CH          (LL_DMA_CHANNEL_3)
#define __SPI2_RX_DMA_CH          (LL_DMA_CHANNEL_4)
#define __SPI2_TX_DMA_CH          (LL_DMA_CHANNEL_5)

#define __SPI1_CLK_ENABLE()       (RCC->APB2ENR |= (1 << 12))
#define __SPI1_CLK_DISABLE()      (RCC->APB2ENR &= ~(1 << 12))
#define __SPI2_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 14))
#define __SPI2_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 14))
#endif

struct stm32_spi
{
    SPI_TypeDef *spi;
    os_msg_t transfer_msg;
    unsigned int lock;
    short rx_dma_ch;
    short tx_dma_ch;
};

enum
{
#if defined(SPI1) && defined(BSP_USING_SPI1)
    SPI1_INDEX,
#endif
#if defined(SPI2) && defined(BSP_USING_SPI2)
    SPI2_INDEX,
#endif
#if defined(SPI3) && defined(BSP_USING_SPI3)
    SPI3_INDEX,
#endif
    SPI_INDEX_MAX
};

static struct stm32_spi spi_list[] = 
{
#if defined(SPI1) && defined(BSP_USING_SPI1)
    {SPI1, {OS_INVALID_VAL, OS_INVALID_VAL}, 0, -1, -1},
#endif
#if defined(SPI2) && defined(BSP_USING_SPI2)
    {SPI2, {OS_INVALID_VAL, OS_INVALID_VAL}, 0, -1, -1},
#endif
#if defined(SPI3) && defined(BSP_USING_SPI3)
    {SPI3, {OS_INVALID_VAL, OS_INVALID_VAL}, 0, -1, -1},
#endif
};

static void spi_clk_enable(unsigned int spi)
{
    switch (spi)
    {
#if defined(SPI1) && defined(BSP_USING_SPI1)
    case SPI1_INDEX:
        __SPI1_CLK_ENABLE();
        break;
#endif
#if defined(SPI2) && defined(BSP_USING_SPI2)
    case SPI2_INDEX:
        __SPI2_CLK_ENABLE();
        break;
#endif
#if defined(SPI3) && defined(BSP_USING_SPI3)
    case SPI3_INDEX:
        __SPI3_CLK_ENABLE();
        break;
#endif
    default:
        break;
    }
    // enable dma
#if defined(BSP_SPI1_RX_USING_DMA) || defined(BSP_SPI1_TX_USING_DMA) \
 || defined(BSP_SPI2_RX_USING_DMA) || defined(BSP_SPI2_TX_USING_DMA) \
 || defined(BSP_SPI3_RX_USING_DMA) || defined(BSP_SPI3_TX_USING_DMA) 
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
#endif
}

void spi_init(spi_handle_t *dev, unsigned int spi_id, unsigned int cs_pin, unsigned int freq, unsigned int mode)
{
    struct stm32_spi *spi;
    LL_SPI_InitTypeDef spi_init;
    unsigned int dma_mdatasize;
    unsigned int dma_pdatasize;

    LOG_ASSERT(spi_id < SPI_INDEX_MAX);
    
    spi = &spi_list[spi_id];
    dev->user_data = spi;
    dev->cs_pin = cs_pin;
    if (LL_SPI_IsEnabled(spi->spi))
        return;

    spi_clk_enable(spi_id);

    if (mode & SPI_SLAVE)
        spi_init.Mode = LL_SPI_MODE_SLAVE;
    else
        spi_init.Mode = LL_SPI_MODE_MASTER;

    if (mode & SPI_LSB)
        spi_init.BitOrder = LL_SPI_LSB_FIRST;
    else 
        spi_init.BitOrder = LL_SPI_MSB_FIRST;

    if (mode & SPI_3WIRE)
        spi_init.TransferDirection = LL_SPI_HALF_DUPLEX_TX;
    else 
        spi_init.TransferDirection = LL_SPI_FULL_DUPLEX;
    
    if (mode & SPI_DATA_16BIT)
    {
        dma_mdatasize = LL_DMA_MDATAALIGN_HALFWORD;
        dma_pdatasize = LL_DMA_PDATAALIGN_HALFWORD;
        spi_init.DataWidth = LL_SPI_DATAWIDTH_16BIT;
    }
    else 
    {
        dma_mdatasize = LL_DMA_MDATAALIGN_BYTE;
        dma_pdatasize = LL_DMA_PDATAALIGN_BYTE;
        spi_init.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    }
    
    if (mode & SPI_CPHA)
        spi_init.ClockPhase = LL_SPI_PHASE_2EDGE;
    else 
        spi_init.ClockPhase = LL_SPI_PHASE_1EDGE;
    
    if (mode & SPI_CPOL)
        spi_init.ClockPolarity = LL_SPI_POLARITY_HIGH;
    else 
        spi_init.ClockPolarity = LL_SPI_POLARITY_LOW;
    
    spi_init.NSS = LL_SPI_NSS_SOFT;

    if (freq >= SystemCoreClock / 2)
    {
        spi_init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
    }
    else if (freq >= SystemCoreClock / 4)
    {
        spi_init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
    }
    else if (freq >= SystemCoreClock / 8)
    {
        spi_init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV8;
    }
    else if (freq >= SystemCoreClock / 16)
    {
        spi_init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV16;
    }
    else if (freq >= SystemCoreClock / 32)
    {
        spi_init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;
    }
    else if (freq >= SystemCoreClock / 64)
    {
        spi_init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV64;
    }
    else if (freq >= SystemCoreClock / 128)
    {
        spi_init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV128;
    }
    else
    {
        spi_init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV256;
    }
    spi_init.CRCCalculation = 0;
    LL_SPI_Init(spi->spi, &spi_init);

#if defined (BSP_SPI1_TX_USING_DMA) || defined (BSP_SPI1_RX_USING_DMA)
    if (spi_id == SPI1_INDEX)
    {
#if defined(BSP_SPI1_RX_USING_DMA)
        // init rx dma
        spi->rx_dma_ch = __SPI1_RX_DMA_CH;
        LL_DMA_SetPeriphRequest(DMA1, __SPI1_RX_DMA_CH, __SPI1_RX_DMA_REQ);
        // LL_DMA_SetDataTransferDirection(DMA1, __SPI1_RX_DMA_CH, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
        // LL_DMA_SetChannelPriorityLevel(DMA1, __SPI1_RX_DMA_CH, LL_DMA_PRIORITY_LOW);
        // LL_DMA_SetMode(DMA1, __SPI1_RX_DMA_CH, LL_DMA_MODE_NORMAL);
        // LL_DMA_SetPeriphIncMode(DMA1, __SPI1_RX_DMA_CH, LL_DMA_PERIPH_NOINCREMENT);
        LL_DMA_SetMemoryIncMode(DMA1, __SPI1_RX_DMA_CH, LL_DMA_MEMORY_INCREMENT);
        LL_DMA_SetPeriphSize(DMA1, __SPI1_RX_DMA_CH, dma_pdatasize);
        LL_DMA_SetMemorySize(DMA1, __SPI1_RX_DMA_CH, dma_mdatasize);
        LL_DMA_EnableIT_TC(DMA1, __SPI1_RX_DMA_CH);
#endif
#if defined (BSP_SPI1_TX_USING_DMA)
        // init tx dma
        spi->tx_dma_ch = __SPI1_TX_DMA_CH;
        LL_DMA_SetPeriphRequest(DMA1, __SPI1_TX_DMA_CH, __SPI1_TX_DMA_REQ);
        LL_DMA_SetDataTransferDirection(DMA1, __SPI1_TX_DMA_CH, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
        // LL_DMA_SetChannelPriorityLevel(DMA1, __SPI1_TX_DMA_CH, LL_DMA_PRIORITY_LOW);
        // LL_DMA_SetMode(DMA1, __SPI1_TX_DMA_CH, LL_DMA_MODE_NORMAL);
        // LL_DMA_SetPeriphIncMode(DMA1, __SPI1_TX_DMA_CH, LL_DMA_PERIPH_NOINCREMENT);
        LL_DMA_SetMemoryIncMode(DMA1, __SPI1_TX_DMA_CH, LL_DMA_MEMORY_INCREMENT);
        LL_DMA_SetPeriphSize(DMA1, __SPI1_TX_DMA_CH, dma_pdatasize);
        LL_DMA_SetMemorySize(DMA1, __SPI1_TX_DMA_CH, dma_mdatasize);
        LL_DMA_EnableIT_TC(DMA1, __SPI1_TX_DMA_CH);
#endif
        //使能DMA中断
        NVIC_SetPriority(__SPI1_DMA_IRQn, 2);
        NVIC_EnableIRQ(__SPI1_DMA_IRQn);
    } else
#endif /* defined (BSP_SPI1_TX_USING_DMA) || defined (BSP_SPI1_RX_USING_DMA) */

#if defined(BSP_SPI2_RX_USING_DMA) || defined(BSP_SPI2_TX_USING_DMA)
    if (spi_id == SPI2_INDEX)
    {
#if defined(BSP_SPI2_RX_USING_DMA)
        // init rx dma
        spi->rx_dma_ch = __SPI2_RX_DMA_CH;
        LL_DMA_SetPeriphRequest(DMA1, __SPI2_RX_DMA_CH, __SPI2_RX_DMA_REQ);
        // LL_DMA_SetDataTransferDirection(DMA1, __SPI2_RX_DMA_CH, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
        // LL_DMA_SetChannelPriorityLevel(DMA1, __SPI2_RX_DMA_CH, LL_DMA_PRIORITY_LOW);
        // LL_DMA_SetMode(DMA1, __SPI2_RX_DMA_CH, LL_DMA_MODE_NORMAL);
        // LL_DMA_SetPeriphIncMode(DMA1, __SPI2_RX_DMA_CH, LL_DMA_PERIPH_NOINCREMENT);
        LL_DMA_SetMemoryIncMode(DMA1, __SPI2_RX_DMA_CH, LL_DMA_MEMORY_INCREMENT);
        LL_DMA_SetPeriphSize(DMA1, __SPI2_RX_DMA_CH, dma_pdatasize);
        LL_DMA_SetMemorySize(DMA1, __SPI2_RX_DMA_CH, dma_mdatasize);
        LL_DMA_EnableIT_TC(DMA1, __SPI2_RX_DMA_CH);
#endif
#if defined (BSP_SPI2_TX_USING_DMA)
        // init tx dma
        spi->tx_dma_ch = __SPI2_TX_DMA_CH;
        LL_DMA_SetPeriphRequest(DMA1, __SPI2_TX_DMA_CH, __SPI2_TX_DMA_REQ);
        LL_DMA_SetDataTransferDirection(DMA1, __SPI2_TX_DMA_CH, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
        // LL_DMA_SetChannelPriorityLevel(DMA1, __SPI2_TX_DMA_CH, LL_DMA_PRIORITY_LOW);
        // LL_DMA_SetMode(DMA1, __SPI2_TX_DMA_CH, LL_DMA_MODE_NORMAL);
        // LL_DMA_SetPeriphIncMode(DMA1, __SPI2_TX_DMA_CH, LL_DMA_PERIPH_NOINCREMENT);
        LL_DMA_SetMemoryIncMode(DMA1, __SPI2_TX_DMA_CH, LL_DMA_MEMORY_INCREMENT);
        LL_DMA_SetPeriphSize(DMA1, __SPI2_TX_DMA_CH, dma_pdatasize);
        LL_DMA_SetMemorySize(DMA1, __SPI2_TX_DMA_CH, dma_mdatasize);
        LL_DMA_EnableIT_TC(DMA1, __SPI2_TX_DMA_CH);
#endif
        //使能DMA中断
        NVIC_SetPriority(__SPI2_DMA_IRQn, 2);
        NVIC_EnableIRQ(__SPI2_DMA_IRQn);
    } else
#endif

#if defined(BSP_SPI3_RX_USING_DMA) || defined(BSP_SPI3_TX_USING_DMA)
    if (spi_id == SPI3_INDEX)
    {
#if defined(BSP_SPI3_RX_USING_DMA)
        // init rx dma
        spi->rx_dma_ch = __SPI3_RX_DMA_CH;
        LL_DMA_SetPeriphRequest(DMA1, __SPI3_RX_DMA_CH, __SPI3_RX_DMA_REQ);
        // LL_DMA_SetDataTransferDirection(DMA1, __SPI3_RX_DMA_CH, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
        // LL_DMA_SetChannelPriorityLevel(DMA1, __SPI3_RX_DMA_CH, LL_DMA_PRIORITY_LOW);
        // LL_DMA_SetMode(DMA1, __SPI3_RX_DMA_CH, LL_DMA_MODE_NORMAL);
        // LL_DMA_SetPeriphIncMode(DMA1, __SPI3_RX_DMA_CH, LL_DMA_PERIPH_NOINCREMENT);
        LL_DMA_SetMemoryIncMode(DMA1, __SPI3_RX_DMA_CH, LL_DMA_MEMORY_INCREMENT);
        LL_DMA_SetPeriphSize(DMA1, __SPI3_RX_DMA_CH, dma_pdatasize);
        LL_DMA_SetMemorySize(DMA1, __SPI3_RX_DMA_CH, dma_mdatasize);
        LL_DMA_EnableIT_TC(DMA1, __SPI3_RX_DMA_CH);
#endif
#if defined (BSP_SPI3_TX_USING_DMA)
        // init tx dma
        spi->tx_dma_ch = __SPI3_TX_DMA_CH;
        LL_DMA_SetPeriphRequest(DMA1, __SPI3_TX_DMA_CH, __SPI3_TX_DMA_REQ);
        LL_DMA_SetDataTransferDirection(DMA1, __SPI3_TX_DMA_CH, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
        // LL_DMA_SetChannelPriorityLevel(DMA1, __SPI3_TX_DMA_CH, LL_DMA_PRIORITY_LOW);
        // LL_DMA_SetMode(DMA1, __SPI3_TX_DMA_CH, LL_DMA_MODE_NORMAL);
        // LL_DMA_SetPeriphIncMode(DMA1, __SPI3_TX_DMA_CH, LL_DMA_PERIPH_NOINCREMENT);
        LL_DMA_SetMemoryIncMode(DMA1, __SPI3_TX_DMA_CH, LL_DMA_MEMORY_INCREMENT);
        LL_DMA_SetPeriphSize(DMA1, __SPI3_TX_DMA_CH, dma_pdatasize);
        LL_DMA_SetMemorySize(DMA1, __SPI3_TX_DMA_CH, dma_mdatasize);
        LL_DMA_EnableIT_TC(DMA1, __SPI3_TX_DMA_CH);
#endif
        //使能DMA中断
        NVIC_SetPriority(__SPI3_DMA_IRQn, 2);
        NVIC_EnableIRQ(__SPI3_DMA_IRQn);
    } else 
#endif
    {
        (void)dma_pdatasize;
        (void)dma_mdatasize;
        // 不使用dma
    }
    LL_SPI_Enable(spi->spi);
}

static void _spi_transfer(struct stm32_spi *spi, void *data, unsigned int size)
{
#if (defined(BSP_USING_SPI1) && defined(BSP_SPI1_TX_USING_DMA) && defined(BSP_SPI1_RX_USING_DMA)) \
 || (defined(BSP_USING_SPI2) && defined(BSP_SPI2_TX_USING_DMA) && defined(BSP_SPI2_RX_USING_DMA)) \
 || (defined(BSP_USING_SPI3) && defined(BSP_SPI3_TX_USING_DMA) && defined(BSP_SPI3_RX_USING_DMA))
    if ((spi->tx_dma_ch >= 0) && (spi->rx_dma_ch >= 0))
    {
        // 设置传输地址和数据大小
        LL_DMA_ConfigAddresses(DMA1, spi->rx_dma_ch, (uint32_t)&spi->spi->DR, (uint32_t)data, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
        LL_DMA_SetDataLength(DMA1, spi->rx_dma_ch, size);
        LL_DMA_ConfigAddresses(DMA1, spi->tx_dma_ch, (uint32_t)data, (uint32_t)&spi->spi->DR, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
        LL_DMA_SetDataLength(DMA1, spi->tx_dma_ch, size);
        //使能DMA，使能DMA传输
        LL_DMA_EnableChannel(DMA1, spi->rx_dma_ch);
        LL_DMA_EnableChannel(DMA1, spi->tx_dma_ch);
        LL_SPI_EnableDMAReq_RX(spi->spi);
        LL_SPI_EnableDMAReq_TX(spi->spi);

        //等待传输完成
        os_msg_recv(&spi->transfer_msg, 0, 1000);

        //关闭DMA传输和DMA
        LL_SPI_DisableDMAReq_TX(spi->spi);
        LL_SPI_DisableDMAReq_RX(spi->spi);
        LL_DMA_DisableChannel(DMA1, spi->tx_dma_ch);
        LL_DMA_DisableChannel(DMA1, spi->rx_dma_ch);
    } else
#endif
    {
        // 不使用dma
        unsigned int sync = 0;
        if (LL_SPI_GetDataWidth(spi->spi) == LL_SPI_DATAWIDTH_16BIT)
        {
            uint16_t *txbuf = data;
            uint16_t *rxbuf = data;
            while(size)
            {
                if ((spi->spi->SR & SPI_SR_TXE) && (sync == 0))
                {
                    spi->spi->DR = *txbuf;
                    txbuf++;
                    sync = 1;
                }

                if (spi->spi->SR & SPI_SR_RXNE)
                {
                    *rxbuf = (uint16_t)spi->spi->DR;
                    rxbuf++;
                    size--;
                    sync = 0;
                }
            }
        }
        else
        {
            uint8_t *txbuf = data;
            uint8_t *rxbuf = data;
            while(size)
            {
                if ((spi->spi->SR & SPI_SR_TXE) && (sync == 0))
                {
                    *((__IO uint8_t*)&spi->spi->DR) = *txbuf;
                    txbuf++;
                    sync = 1;
                }

                if (spi->spi->SR & SPI_SR_RXNE)
                {
                    *rxbuf = *((__IO uint8_t*)&spi->spi->DR);
                    rxbuf++;
                    size--;
                    sync = 0;
                }
            }
        }
    }
}

static void _spi_send(struct stm32_spi *spi, const void *data, unsigned int size)
{
#if (defined(BSP_USING_SPI1) && defined(BSP_SPI1_TX_USING_DMA)) \
 || (defined(BSP_USING_SPI2) && defined(BSP_SPI2_TX_USING_DMA)) \
 || (defined(BSP_USING_SPI3) && defined(BSP_SPI3_TX_USING_DMA)) 
    if (spi->tx_dma_ch >= 0)
    {
        // 设置传输地址和数据大小
        LL_DMA_ConfigAddresses(DMA1, spi->tx_dma_ch, (uint32_t)data, (uint32_t)&spi->spi->DR, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
        LL_DMA_SetDataLength(DMA1, spi->tx_dma_ch, size);
        //使能DMA，使能DMA传输
        LL_DMA_EnableChannel(DMA1, spi->tx_dma_ch);
        LL_SPI_EnableDMAReq_TX(spi->spi);

        //等待传输完成
        os_msg_recv(&spi->transfer_msg, 0, 1000);

        //关闭DMA传输和DMA
        LL_SPI_DisableDMAReq_TX(spi->spi);
        LL_DMA_DisableChannel(DMA1, spi->tx_dma_ch);
    } else
#endif
    {
        // 不使用dma
        if (LL_SPI_GetDataWidth(spi->spi) == LL_SPI_DATAWIDTH_16BIT)
        {
            const uint16_t *txbuf = data;
            while(size)
            {
                if (spi->spi->SR & SPI_SR_TXE)
                {
                    spi->spi->DR = *txbuf;
                    txbuf++;
                    size--;
                }
            }
        }
        else
        {
            const uint8_t *txbuf = data;
            while(size)
            {
                if (spi->spi->SR & SPI_SR_TXE)
                {
                    *((__IO uint8_t*)&spi->spi->DR) = *txbuf;
                    txbuf++;
                    size--;
                }
            }
        }
    }

    // 清空接收缓存
    while(spi->spi->SR & SPI_SR_RXNE)
    {
        __IO unsigned int tmp = spi->spi->DR;
        (void)tmp;
    }
    LL_SPI_ClearFlag_OVR(spi->spi);
}

void spi_transfer(spi_handle_t *dev, void *data, unsigned int size)
{
    LOG_ASSERT(dev != 0);
    struct stm32_spi *spi = dev->user_data;
    LOG_ASSERT(spi != 0);

    while (spi->lock)
        os_delay(1);
    spi->lock = 1;
    pin_write(dev->cs_pin, 0);

    _spi_transfer(spi, data, size);

    pin_write(dev->cs_pin, 1);
    spi->lock = 0;
}

void spi_send(spi_handle_t *dev, const void *data, unsigned int size)
{
    LOG_ASSERT(dev != 0);
    struct stm32_spi *spi = dev->user_data;
    LOG_ASSERT(spi != 0);

    while (spi->lock)
        os_delay(1);
    spi->lock = 1;
    pin_write(dev->cs_pin, 0);

    _spi_send(spi, data, size);

    pin_write(dev->cs_pin, 1);
    spi->lock = 0;
}

void spi_send_then_send(spi_handle_t *dev, const void *send_data1, unsigned int send_size1, const void *send_data2, unsigned int send_size2)
{
    LOG_ASSERT(dev != 0);
    struct stm32_spi *spi = dev->user_data;
    LOG_ASSERT(spi != 0);

    while (spi->lock)
        os_delay(1);
    spi->lock = 1;
    pin_write(dev->cs_pin, 0);

    _spi_send(spi, send_data1, send_size1);
    _spi_send(spi, send_data2, send_size2);

    pin_write(dev->cs_pin, 1);
    spi->lock = 0;
}

void spi_send_then_recv(spi_handle_t *dev, const void *send_data, unsigned int send_size, void *recv_data, unsigned int recv_size)
{
    LOG_ASSERT(dev != 0);
    struct stm32_spi *spi = dev->user_data;
    LOG_ASSERT(spi != 0);

    while (spi->lock)
        os_delay(1);
    spi->lock = 1;
    pin_write(dev->cs_pin, 0);

    _spi_send(spi, send_data, send_size);
    _spi_transfer(spi, recv_data, recv_size);

    pin_write(dev->cs_pin, 1);
    spi->lock = 0;
}

#if defined(BSP_USING_SPI1) && (defined(BSP_SPI1_TX_USING_DMA) || defined(BSP_SPI1_RX_USING_DMA)) 
void DMA1_Channel2_3_IRQHandler(void)
{
    if(LL_DMA_IsActiveFlag_TC2(DMA1))
    {
        os_msg_send(&spi_list[SPI1_INDEX].transfer_msg, 0);
        LL_DMA_ClearFlag_TC2(DMA1);
    }
    if(LL_DMA_IsActiveFlag_TC3(DMA1))
    {
        os_msg_send(&spi_list[SPI1_INDEX].transfer_msg, 0);
        LL_DMA_ClearFlag_TC3(DMA1);
    }
}
#endif

#if defined(BSP_USING_SPI2) && (defined(BSP_SPI2_TX_USING_DMA) || defined(BSP_SPI2_RX_USING_DMA)) 
#if defined(STM32G0)
#if defined(STM32G0B0xx) || defined(STM32G0B1xx) || defined(STM32G0C1xx)
void DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX1_OVR_IRQHandler(void)
#elif defined(STM32G030xx) || defined(STM32G031xx) || defined(STM32G041xx)
void DMA1_Ch4_5_DMAMUX1_OVR_IRQHandler(void)
#else
void DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler(void)
#endif
#else
void DMA1_Channel4_5_6_7_IRQHandler(void)
#endif
{
    if(LL_DMA_IsActiveFlag_TC4(DMA1))
    {
        os_msg_send(&spi_list[SPI2_INDEX].transfer_msg, 0);
        LL_DMA_ClearFlag_TC4(DMA1);
    }
    if(LL_DMA_IsActiveFlag_TC5(DMA1))
    {
        os_msg_send(&spi_list[SPI2_INDEX].transfer_msg, 0);
        LL_DMA_ClearFlag_TC5(DMA1);
    }
}
#endif

#endif
