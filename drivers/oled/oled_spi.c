#include <spi.h>
#include <pin.h>
#include <oled/oled.h>

static void oled_write_cmd(oled_dev_t *oled, unsigned char *cmd, unsigned int size)
{
    pin_write(oled->dc_pin, 0);
    spi_send(oled->handler, cmd, size);
}

static void oled_write_data(oled_dev_t *oled, unsigned char *data, unsigned int size)
{
    pin_write(oled->dc_pin, 1);
    spi_send(oled->handler, data, size);
}

void oled_init(oled_dev_t *oled, void *handler, unsigned int dc_pin, unsigned int dir)
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
        0xAF, // display on
    };

    oled->handler = handler;
    oled->dc_pin = dc_pin;

    pin_mode(dc_pin, PIN_MODE_OUTPUT_PP);

    if (dir)
    {
        init_buf[1] = 0xA0;
        init_buf[2] = 0xC0;
    }

    oled_write_cmd(oled, init_buf, sizeof(init_buf));
}

void oled_draw_point(oled_dev_t *oled, unsigned int x, unsigned int y)
{
    unsigned int pos, bx, temp = 0;

    if (x > 127 || y > 63)
        return;

    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);

    oled->buf[x + pos * 128] |= temp;
}

void oled_draw_v_line(oled_dev_t *oled, unsigned int x, unsigned int y, unsigned int len)
{
    unsigned int pos, bx, temp = 0;

    if (x > 127 || y > 63)
        return;

    pos = (y / 8) * 128;
    bx = y % 8;
    temp = 0xff << bx;
    oled->buf[x + pos] |= temp;

    len -= (8 - bx);
    pos += 128;
    temp = 0xff;

    len = (y + len >= 64) ? 64 - y : len;

    while (len >= 8)
    {
        oled->buf[x + pos] = temp;
        pos += 128;
        len -= 8;
    }

    if (len > 0)
    {
        temp = 0xff >> (8 - len);
        oled->buf[x + pos] |= temp;
    }
}

void oled_draw_h_line(oled_dev_t *oled, unsigned int x, unsigned int y, unsigned int len)
{
    unsigned int i;
    unsigned int pos, bx, temp = 0;

    if (x > 127 || y > 63)
        return;

    pos = (y / 8) * 128;
    bx = y % 8;
    temp = 1 << bx;

    len = (x + len >= 128) ? 128 - x : len;

    for (i = 0; i < len; i++)
    {
        oled->buf[(x++) + pos] |= temp;
    }
}

void oled_draw_rect(oled_dev_t *oled, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    oled_draw_h_line(oled, x, y, w);
    oled_draw_h_line(oled, x, y + h, w);
    oled_draw_v_line(oled, x, y, h);
    oled_draw_v_line(oled, x + w, y, h + 1);
}

void oled_flush(oled_dev_t *oled)
{
    unsigned char cmd[4] = {0xB0, 0x02, 0x10, 0x00};
    for (unsigned int i = 0; i < 8; i++)
    {
        oled_write_cmd(oled, cmd, 3);
        oled_write_data(oled, oled->buf + 128 * i, 128);
        cmd[0]++;
    }
}
