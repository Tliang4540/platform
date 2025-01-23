#include <spi.h>
#include <pin.h>
#include <log.h>
#include <tinyos.h>
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
    unsigned char tmp_buf[4];
    unsigned char data_buf[32];
    unsigned int read_addr = 0;
    spi_hander_t spi;

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

    spi = spi_open(0, 16000000, SPI_MODE0 | SPI_MSB | SPI_MASTER | SPI_4WIRE | SPI_DATA_8BIT);

    tmp_buf[0] = 0x9F;
    pin_write(SPI_CS_PIN, 0);
    spi_transfer(spi, tmp_buf, 4);
    pin_write(SPI_CS_PIN, 1);

    LOG_I("spiflag id:");
    LOG_DUMP(&tmp_buf[1], 3);

    while (1)
    {
        os_delay(200);

        tmp_buf[0] = 0x03;
        tmp_buf[1] = read_addr >> 16;
        tmp_buf[2] = read_addr >> 8;
        tmp_buf[3] = read_addr;
        pin_write(SPI_CS_PIN, 0);
        spi_transfer(spi, tmp_buf, 4);
        spi_transfer(spi, data_buf, 32);
        pin_write(SPI_CS_PIN, 1);
        LOG_I("read %x addr data:\n", read_addr);
        LOG_DUMP(data_buf, 32);
        read_addr += 32;
    }
}

void spi_test_init(void)
{
    os_task_create(spi_test_task, "hello spi test\n", spi_test_stack, sizeof(spi_test_stack));
}
