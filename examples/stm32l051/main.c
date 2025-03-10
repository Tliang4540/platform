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
    timer_hander_t timer;
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
    int x = 0;
    int y = 0;
    int x_dir = 1;
    int y_dir = 1;
    spi_hander_t oled_spi;
    static oled_dev_t oled;

    (void)param;

    pin_speed(29, PIN_SPEED_VERY_HIGH);
    pin_speed(31, PIN_SPEED_VERY_HIGH);
    pin_mode(29, PIN_MODE_FUNCTION_PP);
    pin_mode(31, PIN_MODE_FUNCTION_PP);
    oled_spi = spi_open(1, 16000000, SPI_MODE0 | SPI_MSB | SPI_MASTER | SPI_3WIRE | SPI_DATA_8BIT);
    oled_init(&oled, oled_spi, 30, 0);
    
    while (1)
    {
        memset(oled.buf, 0, sizeof(oled.buf));
        oled_draw_h_line(&oled, 10, 0, 100);
        oled_draw_v_line(&oled, 60, 0, 20);
        oled_draw_rect(&oled, x, y, 20, 20);
        oled_flush(&oled);

        if (x >= 127 - 20)
            x_dir = -1;
        else if (x <= 0)
            x_dir = 1;
        if (y >= 63 - 20)
            y_dir = -1;
        else if (y <= 0)
            y_dir = 1;
        x += x_dir;
        y += y_dir;

        os_delay(20);
    }
}

void pwm_test_timer_callback(void)
{

}

static unsigned int pwm_test_stack[128];
void pwm_test_task(void *param)
{
    unsigned int tmp;
    pwm_hander_t pwm = pwm_open(0);
    adc_hander_t adc = adc_open(0);
    timer_hander_t timer;

    (void)param;
    adc_set_channel(adc, 2);
    timer = timer_open(1, 1000000, pwm_test_timer_callback);

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
    clk_lse_init();

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
