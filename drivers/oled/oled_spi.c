#include <spi.h>
#include <pin.h>
#include <oled/oled.h>
#include <bsp_config.h>

#if defined(BSP_USING_SPI_OLED)
static void oled_write_cmd(oled_dev_t *oled, unsigned char *cmd, unsigned int size)
{
    pin_write(oled->dc_pin, 0);
    spi_send(oled->user_data, cmd, size);
}

static void oled_write_data(oled_dev_t *oled, unsigned char *data, unsigned int size)
{
    pin_write(oled->dc_pin, 1);
    spi_send(oled->user_data, data, size);
}

void oled_init(oled_dev_t *oled, void *user_data, unsigned int dc_pin)
{
    unsigned char init_buf[] = {
        0xAE, // display off
        0xA1, 0xC8,
        // 0x40,
        0x8D, 0x14, // set charge pump
        // 0xD3, 0x00,
        0xDA, 0x12,
        0x81, 0xCF,
        0xD9, 0xF1,
        0xDB, 0x40,
        // 0x20, 0x00,
        // 0x21, 0x00, 0x7F,
        // 0x22, 0x00, 0x07,
        0xA4,
        0xA6,
    };

    oled->user_data = user_data;
    oled->dc_pin = dc_pin;

    pin_mode(dc_pin, PIN_MODE_OUTPUT_PP);

    oled_write_cmd(oled, init_buf, sizeof(init_buf));
}

void oled_display_on(oled_dev_t *oled)
{
    unsigned char cmd[] = {0xAF};
    oled_write_cmd(oled, cmd, 1);
}

void oled_display_off(oled_dev_t *oled)
{
    unsigned char cmd[] = {0xAE};
    oled_write_cmd(oled, cmd, 1);
}

void oled_set_rotate(oled_dev_t *oled, unsigned int dir)
{
    unsigned char cmd[] = {0xA1, 0xC8};
    if (dir)
    {
        cmd[0] = 0xA0;
        cmd[1] = 0xC0;
    }
    oled_write_cmd(oled, cmd, 2);
}

void oled_write(oled_dev_t *oled, unsigned char *data, unsigned int w, unsigned int h)
{
    unsigned char cmd[4] = {0xB0, 0x02, 0x10, 0x00};
    for (unsigned int i = 0; i < (h / 8); i++)
    {
        oled_write_cmd(oled, cmd, 3);
        oled_write_data(oled, data + w * i, w);
        cmd[0]++;
    }
}

#endif
