#ifndef __SPIFLASH_H__
#define __SPIFLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spiflash_dev
{
    void *user_data;
}spiflash_dev_t;

int spiflash_init(spiflash_dev_t *dev, void *user_data);
int spiflash_read(spiflash_dev_t *dev, unsigned int saddr, void *buf, unsigned int size);
int spiflash_write(spiflash_dev_t *dev, unsigned int saddr, const void *buf, unsigned int size);
int spiflash_erase(spiflash_dev_t *dev, unsigned int saddr, unsigned int size);
void spiflash_enter_sleep(spiflash_dev_t *dev);
void spiflash_exit_sleep(spiflash_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif
