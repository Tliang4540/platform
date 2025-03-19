#include <tinyos.h>
#include <log.h>
#include <spi.h>
#include <spiflash/spiflash.h>
#include <bsp_config.h>

#if defined (BSP_USING_SPIFLASH)

#define SPIFLASH_PAGE_SIZE          (256)
#define SPIFLASH_SECTOR_SIZE        (4096)

#define CMD_PAGE_WRITE          0x02
#define CMD_READ                0x03
#define CMD_WRITE_DISABLE       0x04
#define CMD_READ_STATUS         0x05
#define CMD_WRITE_ENABLE        0x06
#define CMD_ERASE_4K            0x20
#define CMD_ERASE_32K           0x52
#define CMD_ERASE_64K           0xD8
#define CMD_ERASE_ALL           0x60

#define CMD_READ_ID             0x9F
#define CMD_DEEP_SLEEP          0xB9
#define CMD_WAKEUP              0xAB

int spiflash_init(spiflash_dev_t *dev, void *user_data)
{
    unsigned char tmp[4];

    tmp[0] = CMD_READ_ID;
    dev->user_data = user_data;
    spi_transfer(dev->user_data, tmp, 4);

    LOG_I("spiflash id:%x %x %x", tmp[1], tmp[2], tmp[3]);
    if (tmp[1] == 0xff)
        return -1;

    return 0;
}

static int spiflash_wait_busy(spiflash_dev_t *dev)
{
    unsigned char buf[4];
    unsigned int overtime = 100;

    while(overtime--)
    {
        buf[0] = CMD_READ_STATUS;
        spi_transfer(dev->user_data, buf, 2);
        if((buf[1] & 0x01) == 0)
            return 0;
        os_delay(1);
    };
    
    return -1;
}

int spiflash_read(spiflash_dev_t *dev, unsigned int saddr, void *buf, unsigned int size)
{
    unsigned char cmd[4];

    if(spiflash_wait_busy(dev))
        return -1;

    cmd[0] = CMD_READ;
    cmd[1] = (unsigned char)(saddr >> 16);
    cmd[2] = (unsigned char)(saddr >> 8);
    cmd[3] = (unsigned char)(saddr);
    
    spi_send_then_recv(dev->user_data, cmd, 4, buf, size);

    return size;
}

int spiflash_write(spiflash_dev_t *dev, unsigned int saddr, const void *buf, unsigned int size)
{
    unsigned int write_size;
    unsigned char cmd[4];

    while(size)
    {
        if(spiflash_wait_busy(dev))
            return -1;

        cmd[0] = CMD_WRITE_ENABLE;
        spi_send(dev->user_data, cmd, 1);

        cmd[0] = CMD_PAGE_WRITE;
        cmd[1] = (unsigned char)(saddr >> 16);
        cmd[2] = (unsigned char)(saddr >> 8);
        cmd[3] = (unsigned char)(saddr);
        
        //获取当前页剩余空间
        write_size = SPIFLASH_PAGE_SIZE - (saddr & (SPIFLASH_PAGE_SIZE - 1));  

        //如果当前页空间足够，写入输入的长度，否则写满整页
        if(write_size > size)
            write_size = size;

        spi_send_then_send(dev->user_data, cmd, 4, buf, write_size);

        buf = (void*)((unsigned int)buf + write_size);
        size -= write_size;
        saddr += write_size;
    }
    return size;
}

int spiflash_erase(spiflash_dev_t *dev, unsigned int saddr, unsigned int size)
{
    unsigned int erase_size;
    unsigned char cmd[4];

    erase_size = (size / SPIFLASH_SECTOR_SIZE);
    if(size & (SPIFLASH_SECTOR_SIZE - 1))
        erase_size++;

    saddr &= ~(SPIFLASH_SECTOR_SIZE - 1);

    while(erase_size--)
    {
        if(spiflash_wait_busy(dev))
            return -1;

        cmd[0] = CMD_WRITE_ENABLE;
        spi_send(dev->user_data, cmd, 1);

        cmd[0] = CMD_ERASE_4K;
        cmd[1] = (unsigned char)(saddr >> 16);
        cmd[2] = (unsigned char)(saddr >> 8);
        cmd[3] = (unsigned char)(saddr);
        spi_send(dev->user_data, cmd, 4);
        saddr += SPIFLASH_SECTOR_SIZE;
    }
    return size;
}

void spiflash_enter_sleep(spiflash_dev_t *dev)
{
    unsigned char cmd[4];
    cmd[0] = CMD_DEEP_SLEEP;
    spi_send(dev->user_data, cmd, 1);
}

void spiflash_exit_sleep(spiflash_dev_t *dev)
{
    unsigned char cmd[4];
    cmd[0] = CMD_WAKEUP;
    spi_send(dev->user_data, cmd, 1);
}

#endif
