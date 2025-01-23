#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct i2c_dev
{
    unsigned short dev_addr;
    unsigned short reserved;
    void *user_data;
}i2c_dev_t;

typedef struct i2c_msg
{
    unsigned int len;
    void *buf;
}i2c_msg_t;

void i2c_open(i2c_dev_t *dev, unsigned int i2c_id, unsigned int dev_addr, unsigned int freq);
void i2c_write(i2c_dev_t *dev, i2c_msg_t *data);
void i2c_read(i2c_dev_t *dev, i2c_msg_t *reg_addr, i2c_msg_t *data);

#ifdef __cplusplus
}
#endif

#endif
