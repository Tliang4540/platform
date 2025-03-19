#ifndef DEVICE_H__
#define DEVICE_H__

#include <object.h>

struct device;

struct device_ops
{
    int (*open)(struct device *);
    int (*close)(struct device *);
    int (*write)(struct device *, const void *, unsigned);
    int (*read)(struct device *, void *, unsigned);
    int (*ioctrl)(struct device *, unsigned, void *);
};

struct device
{
    struct object obj;
    struct device_ops *ops;
    void *user_data;
};

static inline int device_open(struct device *dev)
{
    return dev->ops->open(dev);
}

static inline int device_close(struct device *dev)
{
    return dev->ops->close(dev);
}

static inline int device_write(struct device *dev, const void *data, unsigned size)
{
    return dev->ops->write(dev, data, size);
}

static inline int device_read(struct device *dev, void *data, unsigned size)
{
    return dev->ops->read(dev, data, size);
}

static inline int device_ioctrl(struct device *dev, unsigned cmd, void *data)
{
    return dev->ops->ioctrl(dev, cmd, data);
}

void device_add(struct device *dev, const char *name);
struct device *device_find(const char *name);

#endif
