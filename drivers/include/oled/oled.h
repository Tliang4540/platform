#ifndef __OLED_H__
#define __OLED_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct oled_dev
{
    unsigned int dc_pin;
    void *user_data;
}oled_dev_t;

void oled_init(oled_dev_t *oled, void *user_data, unsigned int dc_pin);
void oled_display_on(oled_dev_t *oled);
void oled_display_off(oled_dev_t *oled);
void oled_set_rotate(oled_dev_t *oled, unsigned int dir);
void oled_write(oled_dev_t *oled, unsigned char *data, unsigned int w, unsigned int h);

#ifdef __cplusplus
}
#endif

#endif
