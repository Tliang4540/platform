#ifndef __OLED_H__
#define __OLED_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct oled_dev
{
    void *handler;
    // unsigned int rst_pin;
    unsigned int dc_pin;
    // unsigned int cs_pin;
    unsigned char buf[128 * 8];
}oled_dev_t;

void oled_init(oled_dev_t *oled, void *handler, unsigned int dc_pin, unsigned int dir);
void oled_flush(oled_dev_t *oled);
void oled_draw_rect(oled_dev_t *oled, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
void oled_draw_h_line(oled_dev_t *oled, unsigned int x, unsigned int y, unsigned int len);
void oled_draw_v_line(oled_dev_t *oled, unsigned int x, unsigned int y, unsigned int len);
void oled_draw_point(oled_dev_t *oled, unsigned int x, unsigned int y);

#ifdef __cplusplus
}
#endif

#endif
