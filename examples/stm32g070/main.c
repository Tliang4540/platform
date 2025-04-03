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
#include <rtc.h>
#include <flash.h>
#include <pm.h>
#include <tinydb.h>

#include "audio_test/audio_test.h"
#include "led_dev/led_dev.h"

#define PM_TEST 0

static unsigned int rtc_wakeup_flag = 0;
void rtc_wakeup_callback(void)
{
    rtc_wakeup_flag = 1;
}

#if !PM_TEST
static unsigned int play_flag = 0;
static unsigned int audio_stack[128];
void os_audio_test(void *param)
{
    unsigned int i = 68;
    LOG_OUT("%s", param);
    audio_test_init();
    while (1)
    {
        os_delay(10);
        if (play_flag)
        {
            audio_test_play(i++);
            if (i > 80)
                i = 68;
            play_flag = 0;
            LOG_I("os_audio_test free stack:%d", os_get_free_stack());
        }
    }
}

static unsigned int i2c_stack[256];
#define TINYDB_TEST 1
#if TINYDB_TEST
static unsigned int db_test[32];
#endif
void os_i2c_test(void *param)
{
    i2c_handle_t i2c;
#if TINYDB_TEST
    tinydb_t tinydb;
    unsigned int test_id = 10;
#endif
    flash_dev_t ocflash;
    unsigned char data[4];

    (void)param;

    onchip_flash_init(&ocflash, 0, 0x08000000 + (64 * 1024), 10 * 2048);

#if TINYDB_TEST
    tinydb_init(&tinydb, &ocflash);
    // tinydb_delete(&tinydb, 0);
    if (tinydb_read(&tinydb, 0, db_test) == 0)
    {
        tinydb_format(&tinydb);
        tinydb_write(&tinydb, 0, db_test, 8);
        tinydb_read(&tinydb, 0, db_test);
    }
    else
    {
        db_test[0]++;
        tinydb_write(&tinydb, 0, db_test, 8);
        tinydb_read(&tinydb, 0, db_test);
    }
    LOG_I("db:%x %x", db_test[0], db_test[1]);
#endif

    pin_mode(31, PIN_MODE_OUTPUT_PP);

    pin_pull(53, PIN_PULL_UP);
    pin_pull(54, PIN_PULL_UP);
    pin_mode(53, PIN_MODE_INPUT);
    pin_mode(54, PIN_MODE_INPUT);

    pin_function(24, 6);
    pin_function(25, 6);
    pin_mode(24, PIN_MODE_FUNCTION_OD); // SCL
    pin_mode(25, PIN_MODE_FUNCTION_OD); // SDA

    i2c_init(&i2c, 0, 0xA0, 100000);
    data[0] = 0x15;
    data[1] = 0x80;

    i2c_write(&i2c, data, 2);

    while (1)
    {
        os_delay(20);

        if (!pin_read(54))
        {
            data[0] = 0x12;
            i2c_read(&i2c, data, 1, data, 2);
            if ((data[0] != 0) || (data[1] != 0))
            {
                LOG_I("i2c read data:%x%x", data[0], data[1]);
                LOG_I("os_i2c_test free stack:%d", os_get_free_stack());
                play_flag = 1;
                os_delay(100);

            #if TINYDB_TEST
                for (unsigned int i = 0; i < 2; i++)
                {
                    if (tinydb_read(&tinydb, 1, db_test))
                    {
                        db_test[0]++;
                    }
                    else
                    {
                        db_test[0] = 0;
                    }
                    tinydb_write(&tinydb, 1, db_test, 32);

                    if (tinydb_read(&tinydb, 2, db_test))
                    {
                        db_test[0]++;
                    }
                    else
                    {
                        db_test[0] = 0;
                    }
                    tinydb_write(&tinydb, 2, db_test, 64);

                    if (tinydb_read(&tinydb, 3, db_test))
                    {
                        db_test[0]++;
                    }
                    else
                    {
                        db_test[0] = 0;
                    }
                    tinydb_write(&tinydb, 3, db_test, 96);

                    if (tinydb_read(&tinydb, 4, db_test))
                    {
                        db_test[0]++;
                    }
                    else
                    {
                        db_test[0] = 0;
                    }
                    tinydb_write(&tinydb, 4, db_test, 128);

                    tinydb_write(&tinydb, test_id, db_test, 128);
                    tinydb_read(&tinydb, test_id, db_test);
                    LOG_I("test:%d", db_test[0]);
                    test_id++;
                }
            #endif 
            }
        }
    }
}

static os_msg_t msg_test;
void timer_test_callback(void)
{
    static unsigned int send_msg = 0;
    os_msg_send(&msg_test, send_msg++);
}

static unsigned int msg_test_stack[64];
void os_msg_test(void *param)
{
    unsigned int ret;
    unsigned int recv_msg;
    timer_handle_t timer;
    (void)param;
    os_msg_init(&msg_test);

    pin_mode(9, PIN_MODE_OUTPUT_PP);
    timer = timer_open(0, 1000, timer_test_callback);
    timer_start(timer, 500);

    while (1)
    {
        ret = os_msg_recv(&msg_test, &recv_msg, 600);
        if (ret == 0)
        {
            pin_write(9, recv_msg & 1);
        }
        else
        {
            LOG_I("os_msg_test free stack:%d", os_get_free_stack());
        }
    }
}

static unsigned int led_stack[84];
void os_led_test(void *param)
{
    struct device *led;
    struct rtc_tm time = {0, 0, 0, 4, 6, 3, 2025};
    unsigned int tmp;

    (void)param;

    led = device_find("led");
    device_open(led);
    rtc_set_time(&time);

    // rtc_wakeup_time_open(1000, rtc_wakeup_callback);
    while (1)
    {
        device_write(led, "\x01", 1);
        os_delay(100);
        device_write(led, "\x00", 1);
        os_delay(400);
        if (rtc_wakeup_flag)
        {
            rtc_get_time(&time);
            tmp = rtc_mktime(&time);
            LOG_I("tamp:%d", tmp);
            rtc_localtime(tmp, &time);
            LOG_I("date:%d-%d-%d %d", time.year, time.mon, time.mday, time.wday);
            LOG_I("time:%d:%d:%d", time.hour, time.min, time.sec);
            LOG_I("os_led_test free stack:%d", os_get_free_stack());
            rtc_wakeup_flag = 0;
        }
    }
}

#else
static unsigned int pin_wakeup_flag = 0;
static void pin_wakeup_callback(void)
{
    pin_wakeup_flag = 1;
}

static unsigned int pm_stack[64];
void os_pm_test(void *param)
{
    struct device *led;
    unsigned int i = 0;
    (void)param;

    led = device_find("led");
    device_open(led);

    pin_pull(54, PIN_PULL_UP);
    pin_mode(54, PIN_MODE_INPUT);

    while (1)
    {
        for (i = 0; i < 10; i++)
        {
            device_write(led, "\x01", 1);
            os_delay(100);
            device_write(led, "\x00", 1);
            os_delay(400);
        }
        LOG_I("enter sleep.%d", rtc_wakeup_flag);
        os_delay(10);
        rtc_wakeup_time_open(1000, rtc_wakeup_callback);
        pin_attach_irq(54, pin_wakeup_callback, PIN_IRQ_MODE_FALLING);
        pin_wakeup_flag = 0;
        i = 0;
        pm_set_mode(PM_MODE_SLEEP);
        while (pin_wakeup_flag == 0)
        {
            pm_enter_sleep();
            i++;
        }
        pm_set_mode(PM_MODE_RUN);
        pin_attach_irq(54, 0, PIN_IRQ_MODE_DISABLE);
        rtc_wakeup_time_close();
        rtc_wakeup_flag = 0;
        LOG_I("sleep time:%d", i);
    }
}
#endif

int main(void)
{
    clk_init();
    clk_lsc_init(CLK_LSC_EXTERNAL);

    pin_mode(2, PIN_MODE_FUNCTION_PP);
    pin_mode(3, PIN_MODE_FUNCTION_PP);
    pin_pull(3, PIN_PULL_UP);
    pin_function(2, 1);
    pin_function(3, 1);
    log_init(1, 500000);
    LOG_I("system startup.");

    rtc_init();

    led_dev_register("led", 8);
    
    #if PM_TEST
    os_task_create(os_pm_test, 0, pm_stack, sizeof(pm_stack));
    #else
    os_task_create(os_led_test, 0, led_stack, sizeof(led_stack));
    os_task_create(os_audio_test, "hello serial.\n", audio_stack, sizeof(audio_stack));
    os_task_create(os_msg_test, 0, msg_test_stack, sizeof(msg_test_stack));
    os_task_create(os_i2c_test, 0, i2c_stack, sizeof(i2c_stack));
    #endif
    systick_init(os_tick_update);
    os_start();
}
