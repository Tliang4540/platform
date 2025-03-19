#include <i2c.h>
#include <stm32.h>
#include <bsp_config.h>
#include <tinyos.h>
#include <log.h>

#if defined(BSP_USING_I2C1) || defined(BSP_USING_I2C2) || defined(BSP_USING_I2C3)

#if defined(STM32G0)
#define __I2C_TIMING_100K         (0x10707DBC)
#define __I2C_TIMING_400K         (0x00602173)

#define __I2C1_IRQn               (23)
#define __I2C2_IRQn               (24)
#define __I2C3_IRQn               (24)

#define __I2C1_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 21))
#define __I2C1_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 21))
#define __I2C2_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 22))
#define __I2C2_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 22))
#define __I2C3_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 23))
#define __I2C3_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 23))
#elif defined (STM32L0) || defined (STM32F0)
#define __I2C_TIMING_100K         (0x10707DBC)
#define __I2C_TIMING_400K         (0x00602173)

#define __I2C1_IRQn               (23)
#define __I2C2_IRQn               (24)

#define __I2C1_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 21))
#define __I2C1_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 21))
#define __I2C2_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 22))
#define __I2C2_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 22))
#endif

struct stm32_i2c
{
    I2C_TypeDef *i2c;
    os_msg_t transfer_msg;
    unsigned short lock;
    unsigned short dev_addr;
    unsigned char *rx_buf;
    const unsigned char *tx_buf;
    unsigned int rx_size;
};

enum
{
#if defined(I2C1) && defined(BSP_USING_I2C1)
    I2C1_INDEX,
#endif
#if defined(I2C2) && defined(BSP_USING_I2C2)
    I2C2_INDEX,
#endif
#if defined(I2C3) && defined(BSP_USING_I2C3)
    I2C3_INDEX,
#endif
    I2C_INDEX_MAX
};

static struct stm32_i2c i2c_list[] = 
{
#if defined(I2C1) && defined(BSP_USING_I2C1)
    {I2C1, {OS_INVALID_VAL, OS_INVALID_VAL}, 0, 0, 0, 0, 0},
#endif
#if defined(I2C2) && defined(BSP_USING_I2C2)
    {I2C2, {OS_INVALID_VAL, OS_INVALID_VAL}, 0, 0, 0, 0, 0},
#endif
#if defined(I2C3) && defined(BSP_USING_I2C3)
    {I2C3, {OS_INVALID_VAL, OS_INVALID_VAL}, 0, 0, 0, 0, 0},
#endif
};

static const unsigned int i2c_irqs[] = 
{
#if defined(I2C1) && defined(BSP_USING_I2C1)
    __I2C1_IRQn,
#endif
#if defined(I2C2) && defined(BSP_USING_I2C2)
    __I2C2_IRQn,
#endif
#if defined(I2C3) && defined(BSP_USING_I2C3)
    __I2C3_IRQn,
#endif
};

static void i2c_clk_enable(unsigned int i2c)
{
    switch (i2c)
    {
#if defined(I2C1) && defined(BSP_USING_I2C1)
    case I2C1_INDEX:
        __I2C1_CLK_ENABLE();
        break;
#endif
#if defined(I2C2) && defined(BSP_USING_I2C2)
    case I2C2_INDEX:
        __I2C2_CLK_ENABLE();
        break;
#endif
#if defined(I2C3) && defined(BSP_USING_I2C3)
    case I2C3_INDEX:
        __I2C3_CLK_ENABLE();
        break;
#endif
    default:
        break;
    }
}

void i2c_init(i2c_handle_t *dev, unsigned int i2c_id, unsigned int dev_addr, unsigned int freq)
{
    struct stm32_i2c *i2c;

    LOG_ASSERT(i2c_id < I2C_INDEX_MAX);

    i2c = &i2c_list[i2c_id];
    dev->dev_addr = dev_addr;
    dev->user_data = i2c;

    if (LL_I2C_IsEnabled(i2c->i2c))
        return;

    i2c_clk_enable(i2c_id);

    if (freq <= 100000)
        freq = __I2C_TIMING_100K;
    else
        freq = __I2C_TIMING_400K;

    LL_I2C_SetTiming(i2c->i2c, freq);
    LL_I2C_Enable(i2c->i2c);

    NVIC_SetPriority(i2c_irqs[i2c_id], 3);
    NVIC_EnableIRQ(i2c_irqs[i2c_id]);
}

int i2c_read(i2c_handle_t *dev, const void *addr, unsigned int addr_size, void *data, unsigned int size)
{
    struct stm32_i2c *i2c;
    unsigned int msg;
    int ret = size;

    LOG_ASSERT(dev != 0);
    i2c = dev->user_data;
    LOG_ASSERT(i2c != 0);

    while(i2c->lock)
        os_delay(1);
    i2c->lock = 1;

    // 准备接收数据
    i2c->rx_buf = data;
    i2c->rx_size = size;
    i2c->dev_addr = dev->dev_addr;

    // 准备发送寄存器地址
    i2c->tx_buf = addr;
    i2c->i2c->TXDR = *i2c->tx_buf++;

    // 开始发送
    LL_I2C_HandleTransfer(i2c->i2c, i2c->dev_addr, LL_I2C_ADDRESSING_MODE_7BIT, addr_size, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);
    i2c->i2c->CR1 |= (I2C_CR1_STOPIE | I2C_CR1_TXIE | I2C_CR1_TCIE);

    if (0 == os_msg_recv(&i2c->transfer_msg, &msg, 1000))
    {
        if (msg)
        {
            LOG_E("i2c rs:%x", msg);
            ret = -1;
        }
    }
    else
    {
        LOG_E("i2c r timeout");
        ret = -1;
    }

    if (i2c->i2c->ISR & I2C_ISR_BUSY)
    {
        i2c->i2c->CR1 = 0;
        LOG_E("i2c rb reset");
        i2c->i2c->CR1 = 1;
    }

    i2c->lock = 0;
    return ret;
}

int i2c_write(i2c_handle_t *dev, const void *data, unsigned int size)
{
    struct stm32_i2c *i2c;
    unsigned int msg;
    int ret = size;

    LOG_ASSERT(dev != 0);
    i2c = dev->user_data;
    LOG_ASSERT(i2c != 0);

    while(i2c->lock)
        os_delay(1);

    i2c->lock = 1;

    i2c->tx_buf = data;
    i2c->i2c->TXDR = *i2c->tx_buf++;

    LL_I2C_HandleTransfer(i2c->i2c, dev->dev_addr, LL_I2C_ADDRESSING_MODE_7BIT, size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
    i2c->i2c->CR1 |= (I2C_CR1_STOPIE | I2C_CR1_TXIE);

    if (0 == os_msg_recv(&i2c->transfer_msg, &msg, 1000))
    {
        if (msg)
        {
            LOG_E("i2c ws:%x", msg);
            ret = -1;
        }
    }
    else
    {
        LOG_E("i2c w timeout");
        ret = -1;
    }

    if (i2c->i2c->ISR & I2C_ISR_BUSY)
    {
        i2c->i2c->CR1 = 0;
        LOG_E("i2c wb reset");
        i2c->i2c->CR1 = 1;
    }

    i2c->lock = 0;
    return ret;
}

void i2c_irq(struct stm32_i2c *i2c)
{
    unsigned int flags = i2c->i2c->ISR;
    unsigned int sources = i2c->i2c->CR1;

    if ((flags & I2C_ISR_RXNE) && (sources & I2C_CR1_RXIE))
    {
        *i2c->rx_buf++ = i2c->i2c->RXDR;
    }
    else if ((flags & I2C_ISR_TXIS) && (sources & I2C_CR1_TXIE))
    {
        i2c->i2c->TXDR = *i2c->tx_buf++;
    }
    else if ((flags & I2C_ISR_TC) && (sources & I2C_CR1_TCIE))
    {
        i2c->i2c->CR1 &= ~(I2C_CR1_TXIE | I2C_CR1_TCIE);
        i2c->i2c->CR1 |= I2C_CR1_RXIE;

        // 一次最多传输255个字节
        LL_I2C_HandleTransfer(i2c->i2c, i2c->dev_addr, LL_I2C_ADDRESSING_MODE_7BIT, i2c->rx_size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
    }

    if ((flags & I2C_ISR_STOPF) && (sources & I2C_CR1_STOPIE))
    {
        if(flags & I2C_ISR_NACKF)
        {
            os_msg_send(&i2c->transfer_msg, i2c->i2c->ISR);
        }
        else
        {
            os_msg_send(&i2c->transfer_msg, 0);
        }
        i2c->i2c->ICR |= (I2C_ICR_ARLOCF | I2C_ICR_BERRCF | I2C_ICR_STOPCF | I2C_ICR_NACKCF);
        i2c->i2c->CR1 &= ~(I2C_CR1_TXIE | I2C_CR1_TCIE | I2C_CR1_RXIE | I2C_CR1_STOPIE);
    }
}

#if defined(I2C1) && defined(BSP_USING_I2C1)
void I2C1_IRQHandler(void)
{
    i2c_irq(&i2c_list[I2C1_INDEX]);
}
#endif

#if defined(I2C2) && defined(BSP_USING_I2C2)
void I2C2_IRQHandler(void)
{
    i2c_irq(&i2c_list[I2C2_INDEX]);
}
#endif

#endif
