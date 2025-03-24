#include <onchip_flash.h>
#include <stm32.h>
#include <log.h>
#include <string.h>
#include <bsp_config.h>

#if defined(STM32G0) && defined(BSP_USING_ONCHIP_FLASH)

#define FLASH_SECTOR_SIZE 2048

#define FLASH_KEY1 0x45670123U
#define FLASH_KEY2 0xCDEF89ABU

void onchip_flash_init(onchip_flash_t *dev, unsigned int base, unsigned int size)
{
    LOG_ASSERT(base % FLASH_SECTOR_SIZE == 0);
    LOG_ASSERT(size % FLASH_SECTOR_SIZE == 0);
    dev->base = base;
    dev->size = size;
}

int onchip_flash_read(onchip_flash_t *dev, unsigned int saddr, void *buf, unsigned int size)
{
    LOG_ASSERT(saddr + size <= dev->size);

    memcpy(buf, (const void*)(saddr + dev->base), size);

    return size;
}

int onchip_flash_write(onchip_flash_t *dev, unsigned int saddr, const void *buf, unsigned int size)
{
    LOG_ASSERT(saddr + size <= dev->size);
    LOG_ASSERT(saddr % 8 == 0);
    LOG_ASSERT(size % 8 == 0);

    unsigned int *p = (unsigned int *)buf;
    unsigned int ret = size;

    saddr = saddr + dev->base;

    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
    FLASH->CR |= FLASH_CR_PG;

    while (size)
    {
        *(unsigned int *)saddr = *p++;
        __ISB();
        *(unsigned int *)(saddr + 4) = *p++;
        saddr += 8;
        size -= 8;

        #if defined(FLASH_DBANK_SUPPORT)
        while (FLASH->SR & (FLASH_SR_BSY1 | FLASH_SR_BSY2));
        #else
        while (FLASH->SR & FLASH_SR_BSY1);
        #endif
        if (FLASH->SR & 0x83FA)
        {
            LOG_E("onchip_flash_write:%x", FLASH->SR);
            ret = -1;
            break;
        }
    }
    FLASH->SR = 0x83FA;
    FLASH->CR &= ~FLASH_CR_PG;
    FLASH->CR |= FLASH_CR_LOCK;

    return ret;
}

int onchip_flash_erase(onchip_flash_t *dev, unsigned int saddr, unsigned int size)
{
    LOG_ASSERT(saddr + size <= dev->size);
    LOG_ASSERT(saddr % FLASH_SECTOR_SIZE == 0);
    LOG_ASSERT(size % FLASH_SECTOR_SIZE == 0);

    unsigned int tmp;
    unsigned int ret = size;
    saddr = (dev->base + saddr) / FLASH_SECTOR_SIZE;
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;

    while(size)
    {
        tmp = FLASH->CR & ~FLASH_CR_PNB;
        FLASH->CR = tmp | (saddr << 3) | FLASH_CR_PER | FLASH_CR_STRT;
        #if defined(FLASH_DBANK_SUPPORT)
        while (FLASH->SR & (FLASH_SR_BSY1 | FLASH_SR_BSY2));
        #else
        while (FLASH->SR & FLASH_SR_BSY1);
        #endif
        if (FLASH->SR & 0x83FA)
        {
            LOG_E("onchip_flash_erase:%x", FLASH->SR);
            ret = -1;
            break;
        }
        saddr++;
        size -= FLASH_SECTOR_SIZE;
    }
    FLASH->SR = 0x83FA;
    FLASH->CR &= ~FLASH_CR_PER;
    FLASH->CR |= FLASH_CR_LOCK;

    return ret;
}

#endif
