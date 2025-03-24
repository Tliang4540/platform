#ifndef __ONCHIP_FLASH_H__
#define __ONCHIP_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct onchip_flash
{
    unsigned int base;
    unsigned int size;
}onchip_flash_t;

void onchip_flash_init(onchip_flash_t *dev, unsigned int base, unsigned int size);
int onchip_flash_read(onchip_flash_t *dev, unsigned int saddr, void *buf, unsigned int size);
int onchip_flash_write(onchip_flash_t *dev, unsigned int saddr, const void *buf, unsigned int size);
int onchip_flash_erase(onchip_flash_t *dev, unsigned int saddr, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif
