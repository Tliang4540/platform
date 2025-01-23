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

#include "led_dev/led_dev.h"

static unsigned int serial_stack[128];
void os_serial_test(void *param)
{
    unsigned char tmp_buf[4];
    unsigned char data_buf[32];
    unsigned int read_addr = 0;
    spi_hander_t spi;

    LOG_OUT("%s", param);
    
    //sck
    pin_speed(19, PIN_SPEED_VERY_HIGH);
    pin_mode(19, PIN_MODE_FUNCTION_PP); 

    pin_speed(20, PIN_SPEED_VERY_HIGH);
    pin_pull(20, PIN_PULL_UP);
    pin_mode(20, PIN_MODE_FUNCTION_PP); //miso

    pin_speed(21, PIN_SPEED_VERY_HIGH);
    pin_mode(21, PIN_MODE_FUNCTION_PP); //mosi

    pin_write(52, 1);
    pin_mode(52, PIN_MODE_OUTPUT_PP); //cs

    spi = spi_open(0, 32000000, SPI_MODE0 | SPI_MSB | SPI_MASTER | SPI_4WIRE | SPI_DATA_8BIT);

    tmp_buf[0] = 0x9F;
    pin_write(52, 0);
    spi_transfer(spi, tmp_buf, 4);
    pin_write(52, 1);

    LOG_I("spiflag id:");
    LOG_DUMP(&tmp_buf[1], 3);

    while (1)
    {
        os_delay(200);

        tmp_buf[0] = 0x03;
        tmp_buf[1] = read_addr >> 16;
        tmp_buf[2] = read_addr >> 8;
        tmp_buf[3] = read_addr;
        pin_write(52, 0);
        spi_transfer(spi, tmp_buf, 4);
        spi_transfer(spi, data_buf, 32);
        pin_write(52, 1);
        LOG_I("read %x addr data:\n", read_addr);
        LOG_DUMP(data_buf, 32);
        read_addr += 32;
    }
}

static unsigned int i2c_stack[64];
void os_i2c_test(void *param)
{
    i2c_dev_t dev;
    i2c_msg_t data_msg;
    i2c_msg_t addr_msg;
    unsigned char data[4];

    (void)param;

    pin_mode(31, PIN_MODE_OUTPUT_PP);

    pin_pull(53, PIN_PULL_UP);
    pin_pull(54, PIN_PULL_UP);
    pin_mode(53, PIN_MODE_INPUT);
    pin_mode(54, PIN_MODE_INPUT);

    pin_function(24, 6);
    pin_function(25, 6);
    pin_mode(24, PIN_MODE_FUNCTION_OD); // SCL
    pin_mode(25, PIN_MODE_FUNCTION_OD); // SDA

    i2c_open(&dev, 0, 0xA0, 100000);
    data[0] = 0x15;
    data[1] = 0x80;
    data_msg.buf = data;
    data_msg.len = 2;

    addr_msg.buf = data;
    addr_msg.len = 1;

    i2c_write(&dev, &data_msg);

    while (1)
    {
        os_delay(20);

        if (!pin_read(54))
        {
            data[0] = 0x12;
            i2c_read(&dev, &addr_msg, &data_msg);
            if ((data[0] != 0) || (data[1] != 0))
            {
                LOG_I("i2c read data:%x%x\n", data[0], data[1]);
                os_delay(100);
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
    timer_hander_t timer;
    (void)param;
    os_msg_init(&msg_test);

    pin_mode(9, PIN_MODE_OUTPUT_PP);
    timer = timer_open(0, 1000, timer_test_callback);
    timer_start(timer, 500);

    while (1)
    {
        ret = os_msg_recv(&msg_test, &recv_msg, 300);
        if (ret == 0)
        {
            LOG_I("recv msg:%d\n", recv_msg);
            pin_write(9, recv_msg & 1);
        }
        else
        {
            LOG_I("recv timeout.\n");
        }
    }
}

static unsigned int led_stack[64];
void os_led_test(void *param)
{
    struct device *led;

    (void)param;

    led = device_find("led");
    device_open(led);

    while (1)
    {
        device_write(led, "\x01", 1);
        os_delay(100);
        device_write(led, "\x00", 1);
        os_delay(400);
    }
}

int main(void)
{
    clk_init();

    pin_mode(2, PIN_MODE_FUNCTION_PP);
    pin_mode(3, PIN_MODE_FUNCTION_PP);
    pin_pull(3, PIN_PULL_UP);
    pin_function(2, 1);
    pin_function(3, 1);
    log_init(1, 115200);
    LOG_I("system startup.\n");

    led_dev_register("led", 8);

    os_task_create(os_led_test, 0, led_stack, sizeof(led_stack));
    os_task_create(os_serial_test, "hello serial.\n", serial_stack, sizeof(serial_stack));
    os_task_create(os_msg_test, 0, msg_test_stack, sizeof(msg_test_stack));
    os_task_create(os_i2c_test, 0, i2c_stack, sizeof(i2c_stack));
    systick_init(os_tick_update);
    os_start();
}
