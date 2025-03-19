#include <pin.h>
#include <clk.h>
#include <serial.h>
#include <systick.h>
#include <device.h>
#include <timer.h>
#include <log.h>
#include <tinyos.h>
#include <spi.h>
#include <i2c.h>
#include <oled/oled.h>
#include <string.h>
#include <pwm.h>
#include <adc.h>
#include <rtc.h>
#include <graphic.h>

#include "spi_test/spi_test.h"
#include "led_dev/led_dev.h"

static os_msg_t msg_test;
void timer_test_callback(void)
{
    static unsigned int send_msg = 0;
    os_msg_send(&msg_test, send_msg++);
}

static unsigned int wakeup_flag = 0;
static void rtc_wakeup_callback(void)
{
    wakeup_flag = 1;
}

static unsigned int msg_test_stack[128];
void os_msg_test(void *param)
{
    struct device *led;
    struct rtc_tm time = {0, 0, 0, 4, 6, 3, 2025};
    unsigned int ret;
    unsigned int recv_msg;
    timer_handle_t timer;
    (void)param;
    os_msg_init(&msg_test);
    rtc_set_time(&time);
    led = device_find("led");
    device_open(led);

    // 定时器1KHz运行
    timer = timer_open(0, 1000, timer_test_callback);
    // 定时器500个周期运行
    timer_start(timer, 500);

    while (1)
    {
        ret = os_msg_recv(&msg_test, &recv_msg, 300);
        if (ret == 0)
        {
            LOG_I("recv msg:%d", recv_msg);

            if (recv_msg % 2)
                device_write(led, "\x01", 1);
            else 
                device_write(led, "\x00", 1);
        }
        else
        {
            LOG_I("recv timeout.");
        }
        rtc_get_time(&time);
        ret = rtc_mktime(&time);
        LOG_I("tamp:%d", ret);
        rtc_localtime(ret, &time);
        LOG_I("date:%d-%d-%d %d", time.year, time.mon, time.mday, time.wday);
        LOG_I("time:%d:%d:%d", time.hour, time.min, time.sec);
        if (wakeup_flag)
        {
            LOG_I("wakeup.");
            wakeup_flag = 0;
        }
    }
}

static unsigned int led_stack[128];
void os_led_test(void *param)
{
    unsigned char fill = 0xff;
    unsigned int logic = 0;
    spi_handle_t spi;
    oled_dev_t oled;
    static unsigned char display_buf[1024];
    graphic_t graphic;

    (void)param;

    pin_speed(29, PIN_SPEED_VERY_HIGH);
    pin_speed(31, PIN_SPEED_VERY_HIGH);
    pin_mode(29, PIN_MODE_FUNCTION_PP);
    pin_mode(31, PIN_MODE_FUNCTION_PP);
    spi_init(&spi, 1, 32, 16000000, SPI_MODE0 | SPI_MSB | SPI_MASTER | SPI_3WIRE | SPI_DATA_8BIT);
    oled_init(&oled, &spi, 30);
    oled_display_on(&oled);
    graphic_init(&graphic, 128, 64, display_buf);
    
    while (1)
    {
        if (fill == 0)
            fill = 0xff;
        else
            fill = 0;
        
        graphic_fill(&graphic, fill);
        graphic_set_logic(&graphic, logic);
        logic = (logic + 1) % 3;
        for (int i = 0; i < 68; i+=4)
        {
            graphic_draw_line(&graphic, &(point_t){0, 0}, &(point_t){127, i});
            oled_write(&oled, display_buf, 128, 64);
            os_delay(100);
        }
        for (int i = 120; i > -8; i-=8)
        {
            graphic_draw_line(&graphic, &(point_t){0, 0}, &(point_t){i, 63});
            oled_write(&oled, display_buf, 128, 64);
            os_delay(100);
        }
        graphic_fill(&graphic, fill);
        for (int i = 0; i < 68; i+=4)
        {
            graphic_draw_line(&graphic, &(point_t){127, 0}, &(point_t){0, i});
            oled_write(&oled, display_buf, 128, 64);
            os_delay(100);
        }
        for (int i = 7; i < 135; i+=8)
        {
            graphic_draw_line(&graphic, &(point_t){127, 0}, &(point_t){i, 63});
            oled_write(&oled, display_buf, 128, 64);
            os_delay(100);
        }
        graphic_fill(&graphic, fill);
        for (int i = 0; i < 136; i+=8)
        {
            graphic_draw_line(&graphic, &(point_t){0, 63}, &(point_t){i, 0});
            oled_write(&oled, display_buf, 128, 64);
            os_delay(100);
        }
        for (int i = 3; i < 67; i+=4)
        {
            graphic_draw_line(&graphic, &(point_t){0, 63}, &(point_t){127, i});
            oled_write(&oled, display_buf, 128, 64);
            os_delay(100);
        }
        graphic_fill(&graphic, fill);
        for (int i = 63; i > -3; i-=4)
        {
            graphic_draw_line(&graphic, &(point_t){127, 63}, &(point_t){0, i});
            oled_write(&oled, display_buf, 128, 64);
            os_delay(100);
        }
        for (int i = 7; i < 135; i+=8)
        {
            graphic_draw_line(&graphic, &(point_t){127, 63}, &(point_t){i, 0});
            oled_write(&oled, display_buf, 128, 64);
            os_delay(100);
        }
        graphic_fill(&graphic, fill);
        for (int i = 0; i < 32; i+=4)
        {
            graphic_draw_h_line(&graphic, &(point_t){0, i}, 128);
            graphic_draw_h_line(&graphic, &(point_t){127, 63 - i}, -128);
            oled_write(&oled, display_buf, 128, 64);
            os_delay(100);
        }
        for (int i = 0; i < 64; i+=4)
        {
            graphic_draw_v_line(&graphic, &(point_t){i, 0}, 64);
            graphic_draw_v_line(&graphic, &(point_t){127 - i, 63}, -64);
            oled_write(&oled, display_buf, 128, 64);
            os_delay(100);
        }
        graphic_fill(&graphic, fill);
        for (int i = 2; i < 128; i+=16)
        {
            graphic_draw_rect(&graphic, &(rect_t){i, 2, 12, 12});
            graphic_fill_rect(&graphic, &(rect_t){i, 18, 12, 12});
            graphic_draw_rect(&graphic, &(rect_t){i, 34, 12, 12});
            graphic_fill_rect(&graphic, &(rect_t){i, 50, 12, 12});
            oled_write(&oled, display_buf, 128, 64);
            os_delay(500);
        }
    }
}

static unsigned int pwm_test_stack[128];
void pwm_test_task(void *param)
{
    unsigned int tmp;
    pwm_handle_t pwm = pwm_open(0);
    adc_handle_t adc = adc_open(0);
    timer_handle_t timer;

    (void)param;
    adc_set_channel(adc, 2);
    timer = timer_open(1, 1000000, 0);

    while (1)
    {
        pwm_set(pwm, 0, 1000, 800);
        pwm_set(pwm, 1, 1000, 200);
        pwm_enable(pwm, 0);
        pwm_enable(pwm, 1);

        timer_start(timer, 50000);
        tmp = adc_read(adc);
        timer_stop(timer);
        LOG_I("adc:%d, timer:%d", tmp, timer_read(timer));

        os_delay(500);
        pwm_set(pwm, 0, 10000, 8000);
        pwm_set(pwm, 1, 10000, 2000);

        timer_start(timer, 50000);
        tmp = adc_read(adc);
        timer_stop(timer);
        LOG_I("adc:%d, timer:%d", tmp, timer_read(timer));

        os_delay(500);
        pwm_disable(pwm, 0);
        pwm_disable(pwm, 1);

        timer_start(timer, 50000);
        tmp = adc_read(adc);
        timer_stop(timer);
        LOG_I("adc:%d, timer:%d", tmp, timer_read(timer));

        os_delay(500);
    }
}

int main(void)
{
    clk_init();
    clk_lsc_init(CLK_LSC_EXTERNAL);

    pin_mode(9, PIN_MODE_FUNCTION_PP);
    pin_mode(10, PIN_MODE_FUNCTION_PP);
    pin_pull(10, PIN_PULL_UP);
    pin_function(9, 4);
    pin_function(10, 4);
    log_init(0, 500000);
    LOG_I("system startup.");

    led_dev_register("led", 16);

    pin_mode(0, PIN_MODE_FUNCTION_PP);
    pin_function(0, 2);
    pin_mode(1, PIN_MODE_FUNCTION_PP);
    pin_function(1, 2);

    rtc_init();
    rtc_wakeup_time_open(1000, rtc_wakeup_callback);

    spi_test_init();

    os_task_create(os_led_test, 0, led_stack, sizeof(led_stack));
    os_task_create(os_msg_test, 0, msg_test_stack, sizeof(msg_test_stack));
    os_task_create(pwm_test_task, 0, pwm_test_stack, sizeof(pwm_test_stack));
    systick_init(os_tick_update);
    os_start();
}
