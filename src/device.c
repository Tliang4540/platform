#include <device.h>
#include <string.h>

static struct list_head device_list = {&device_list, &device_list};

struct device *device_find(const char *name)
{
    struct list_head *pos;
    struct device *dev;

    list_for_each(pos, &device_list)
    {
        dev = (struct device*)pos;
        if (strcmp(dev->obj.name, name) == 0)
        {
            return dev;
        }
    }

    return 0;
}

void device_add(struct device *dev, const char *name)
{
    object_init((struct object*)dev, name);

    list_add((struct list_head*)dev, &device_list);
}
