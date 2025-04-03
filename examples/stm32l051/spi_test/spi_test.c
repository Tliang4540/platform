#include <spi.h>
#include <pin.h>
#include <log.h>
#include <tinyos.h>
#include <flash.h>
#include "spi_test.h"

#define SPI_CS_PIN          4
#define SPI_SCK_PIN         5
#define SPI_MISO_PIN        6
#define SPI_MOSI_PIN        7

#define OLED_SPI_SCK_PIN    29
#define OLED_SPI_MOSI_PIN   31
#define OLED_SPI_DC_PIN     30

static unsigned int spi_test_stack[128];
static void spi_test_task(void *param)
{
    unsigned char data_buf[32];
    unsigned int read_addr = 0;
    spi_handle_t spi;
    flash_dev_t spiflash;

    LOG_OUT("%s", param);
    
    //sck
    pin_speed(SPI_SCK_PIN, PIN_SPEED_VERY_HIGH);
    pin_mode(SPI_SCK_PIN, PIN_MODE_FUNCTION_PP); 

    pin_speed(SPI_MISO_PIN, PIN_SPEED_VERY_HIGH);
    pin_pull(SPI_MISO_PIN, PIN_PULL_UP);
    pin_mode(SPI_MISO_PIN, PIN_MODE_FUNCTION_PP); //miso

    pin_speed(SPI_MOSI_PIN, PIN_SPEED_VERY_HIGH);
    pin_mode(SPI_MOSI_PIN, PIN_MODE_FUNCTION_PP); //mosi

    pin_write(SPI_CS_PIN, 1);
    pin_mode(SPI_CS_PIN, PIN_MODE_OUTPUT_PP); //cs

    spi_init(&spi, 0, 4, 16000000, SPI_MODE0 | SPI_MSB | SPI_MASTER | SPI_4WIRE | SPI_DATA_8BIT);
    spiflash_init(&spiflash, &spi, 0, 8 * 1024 * 1024);

    while (1)
    {
        os_delay(200);

        flash_read(&spiflash, read_addr, data_buf, 32);
        LOG_I("read %x addr data:", read_addr);
        LOG_DUMP(data_buf, 32);
        read_addr += 32;
    }
}

void spi_test_init(void)
{
    os_task_create(spi_test_task, "hello spi test\n", spi_test_stack, sizeof(spi_test_stack));
}
