#include <device.h>
#include <pin.h>

static int led_open(struct device *dev)
{
    pin_mode((int)dev->user_data, PIN_MODE_OUTPUT_PP);
    return 0;
}

static int led_write(struct device *dev, const void *data, unsigned size)
{
    (void)(size);
    pin_write((int)dev->user_data, *(unsigned char*)data);

    return 0;
}

static struct device_ops led_ops = {
    .open = led_open,
    .write = led_write,
};

static struct device led_dev = {
    .ops = &led_ops,
};

void led_dev_register(const char *name, unsigned int pin)
{
    led_dev.user_data = (void*)pin;
    device_add(&led_dev, name);
}
