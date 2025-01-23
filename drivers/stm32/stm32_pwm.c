#include <pwm.h>
#include <stm32.h>
#include <bsp_config.h>
#include <log.h>

#if defined(BSP_USING_TIMER1) || defined(BSP_USING_TIMER2) || defined(BSP_USING_TIMER3) \
 || defined(BSP_USING_TIMER4) || defined(BSP_USING_TIMER5) || defined(BSP_USING_TIMER6) \
 || defined(BSP_USING_TIMER7) || defined(BSP_USING_TIMER14) || defined(BSP_USING_TIMER15) \
 || defined(BSP_USING_TIMER16) || defined(BSP_USING_TIMER17) || defined(BSP_USING_TIMER21) \
 || defined(BSP_USING_TIMER22)

#if defined(STM32G0)
#define __TIM1_IRQn               (13)
#define __TIM2_IRQn               (15)
#define __TIM3_IRQn               (16)
#define __TIM4_IRQn               (16)
#define __TIM6_IRQn               (17)
#define __TIM7_IRQn               (18)
#define __TIM14_IRQn              (19)
#define __TIM15_IRQn              (20)
#define __TIM16_IRQn              (21)
#define __TIM17_IRQn              (22)

#define __TIM1_CLK_ENABLE()       (RCC->APBENR2 |= (1 << 11))
#define __TIM1_CLK_DISABLE()      (RCC->APBENR2 &= ~(1 << 11))
#define __TIM2_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 0))
#define __TIM2_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 0))
#define __TIM3_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 1))
#define __TIM3_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 1))
#define __TIM4_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 2))
#define __TIM4_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 2))
#define __TIM6_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 4))
#define __TIM6_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 4))
#define __TIM7_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 5))
#define __TIM7_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 5))
#define __TIM14_CLK_ENABLE()      (RCC->APBENR2 |= (1 << 15))
#define __TIM14_CLK_DISABLE()     (RCC->APBENR2 &= ~(1 << 15))
#define __TIM15_CLK_ENABLE()      (RCC->APBENR2 |= (1 << 16))
#define __TIM15_CLK_DISABLE()     (RCC->APBENR2 &= ~(1 << 16))
#define __TIM16_CLK_ENABLE()      (RCC->APBENR2 |= (1 << 17))
#define __TIM16_CLK_DISABLE()     (RCC->APBENR2 &= ~(1 << 17))
#define __TIM17_CLK_ENABLE()      (RCC->APBENR2 |= (1 << 18))
#define __TIM17_CLK_DISABLE()     (RCC->APBENR2 &= ~(1 << 18))
#elif defined (STM32L0)
#define __TIM2_IRQn               (15)
#define __TIM3_IRQn               (16)
#define __TIM6_IRQn               (17)
#define __TIM7_IRQn               (18)
#define __TIM21_IRQn              (20)
#define __TIM22_IRQn              (22)

#define __TIM2_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 0))
#define __TIM2_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 0))
#define __TIM3_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 1))
#define __TIM3_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 1))
#define __TIM6_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 4))
#define __TIM6_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 4))
#define __TIM7_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 5))
#define __TIM7_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 5))
#define __TIM21_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 2))
#define __TIM21_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 2))
#define __TIM22_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 5))
#define __TIM22_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 5))
#elif defined (STM32F0)
#define __TIM1_IRQn               (13)
#define __TIM2_IRQn               (15)
#define __TIM3_IRQn               (16)
#define __TIM6_IRQn               (17)
#define __TIM7_IRQn               (18)
#define __TIM14_IRQn              (19)
#define __TIM15_IRQn              (20)
#define __TIM16_IRQn              (21)
#define __TIM17_IRQn              (22)

#define __TIM1_CLK_ENABLE()       (RCC->APB2ENR |= (1 << 11))
#define __TIM1_CLK_DISABLE()      (RCC->APB2ENR &= ~(1 << 11))
#define __TIM2_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 0))
#define __TIM2_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 0))
#define __TIM3_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 1))
#define __TIM3_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 1))
#define __TIM6_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 4))
#define __TIM6_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 4))
#define __TIM7_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 5))
#define __TIM7_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 5))
#define __TIM14_CLK_ENABLE()      (RCC->APB1ENR |= (1 << 8))
#define __TIM14_CLK_DISABLE()     (RCC->APB1ENR &= ~(1 << 8))
#define __TIM15_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 16))
#define __TIM15_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 16))
#define __TIM16_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 17))
#define __TIM16_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 17))
#define __TIM17_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 18))
#define __TIM17_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 18))
#endif

struct stm32_pwm
{
    TIM_TypeDef *tim;
    IRQn_Type irq;
};

enum
{
#if defined(TIM1) && defined(BSP_USING_TIMER1)
    TIMER1_INDEX,
#endif
#if defined(TIM2) && defined(BSP_USING_TIMER2)
    TIMER2_INDEX,
#endif
#if defined(TIM3) && defined(BSP_USING_TIMER3)
    TIMER3_INDEX,
#endif
#if defined(TIM4) && defined(BSP_USING_TIMER4)
    TIMER4_INDEX,
#endif
#if defined(TIM6) && defined(BSP_USING_TIMER6)
    TIMER6_INDEX,
#endif
#if defined(TIM7) && defined(BSP_USING_TIMER7)
    TIMER7_INDEX,
#endif
#if defined(TIM14) && defined(BSP_USING_TIMER14)
    TIMER14_INDEX,
#endif
#if defined(TIM15) && defined(BSP_USING_TIMER15)
    TIMER15_INDEX,
#endif
#if defined(TIM16) && defined(BSP_USING_TIMER16)
    TIMER16_INDEX,
#endif
#if defined(TIM17) && defined(BSP_USING_TIMER17)
    TIMER17_INDEX,
#endif
#if defined(TIM21) && defined(BSP_USING_TIMER21)
    TIMER21_INDEX,
#endif
#if defined(TIM22) && defined(BSP_USING_TIMER22)
    TIMER22_INDEX,
#endif
    TIMER_INDEX_MAX
};

static struct stm32_pwm pwm_list[] = 
{
#if defined(TIM1) && defined(BSP_USING_TIMER1)
    {TIM1, __TIM1_IRQn, 0},
#endif
#if defined(TIM2) && defined(BSP_USING_TIMER2)
    {TIM2, __TIM2_IRQn, 0},
#endif
#if defined(TIM3) && defined(BSP_USING_TIMER3)
    {TIM3, __TIM3_IRQn, 0},
#endif
#if defined(TIM4) && defined(BSP_USING_TIMER4)
    {TIM4, __TIM4_IRQn, 0},
#endif
#if defined(TIM6) && defined(BSP_USING_TIMER6)
    {TIM6, __TIM6_IRQn, 0},
#endif
#if defined(TIM7) && defined(BSP_USING_TIMER7)
    {TIM7, __TIM7_IRQn, 0},
#endif
#if defined(TIM14) && defined(BSP_USING_TIMER14)
    {TIM14, __TIM14_IRQn, 0},
#endif
#if defined(TIM15) && defined(BSP_USING_TIMER15)
    {TIM15, __TIM15_IRQn, 0},
#endif
#if defined(TIM16) && defined(BSP_USING_TIMER16)
    {TIM16, __TIM16_IRQn, 0},
#endif
#if defined(TIM17) && defined(BSP_USING_TIMER17)
    {TIM17, __TIM17_IRQn, 0},
#endif
#if defined(TIM21) && defined(BSP_USING_TIMER21)
    {TIM21, __TIM21_IRQn, 0},
#endif
#if defined(TIM22) && defined(BSP_USING_TIMER22)
    {TIM22, __TIM22_IRQn, 0},
#endif
};

static void pwm_clk_enable(unsigned int timerid)
{
    switch (timerid)
    {
#if defined(TIM1) && defined(BSP_USING_TIMER1)
    case TIMER1_INDEX:
        __TIM1_CLK_ENABLE();
        break;
#endif
#if defined(TIM2) && defined(BSP_USING_TIMER2)
    case TIMER2_INDEX:
        __TIM2_CLK_ENABLE();
        break;
#endif
#if defined(TIM3) && defined(BSP_USING_TIMER3)
    case TIMER3_INDEX:
        __TIM3_CLK_ENABLE();
        break;
#endif
#if defined(TIM4) && defined(BSP_USING_TIMER4)
    case TIMER4_INDEX:
        __TIM4_CLK_ENABLE();
        break;
#endif
#if defined(TIM6) && defined(BSP_USING_TIMER6)
    case TIMER6_INDEX:
        __TIM6_CLK_ENABLE();
        break;
#endif
#if defined(TIM7) && defined(BSP_USING_TIMER7)
    case TIMER7_INDEX:
        __TIM7_CLK_ENABLE();
        break;
#endif
#if defined(TIM14) && defined(BSP_USING_TIMER14)
    case TIMER14_INDEX:
        __TIM14_CLK_ENABLE();
        break;
#endif
#if defined(TIM15) && defined(BSP_USING_TIMER15)
    case TIMER15_INDEX:
        __TIM15_CLK_ENABLE();
        break;
#endif
#if defined(TIM16) && defined(BSP_USING_TIMER16)
    case TIMER16_INDEX:
        __TIM16_CLK_ENABLE();
        break;
#endif
#if defined(TIM17) && defined(BSP_USING_TIMER17)
    case TIMER17_INDEX:
        __TIM17_CLK_ENABLE();
        break;
#endif
#if defined(TIM21) && defined(BSP_USING_TIMER21)
    case TIMER21_INDEX:
        __TIM21_CLK_ENABLE();
        break;
#endif
#if defined(TIM22) && defined(BSP_USING_TIMER22)
    case TIMER22_INDEX:
        __TIM22_CLK_ENABLE();
        break;
#endif
    default:
        break;
    }
}

pwm_hander_t pwm_open(unsigned int timerid, unsigned int freq, unsigned int duty)
{
    struct stm32_pwm *timer;

    LOG_ASSERT(timerid < TIMER_INDEX_MAX);

    timer = &pwm_list[timerid];

    pwm_clk_enable(timerid);

    timer->tim->PSC = SystemCoreClock / freq - 1;
    timer->tim->SR = 0;

    NVIC_SetPriority(timer->irq, 3);
    NVIC_EnableIRQ(timer->irq);

    return timer;
}

#endif

