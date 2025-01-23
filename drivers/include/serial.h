#ifndef __SERIAL_H__
#define __SERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void* serial_hander_t;

serial_hander_t serial_open(unsigned int serial_id, unsigned int baudrate);
void serial_close(serial_hander_t serial);
void serial_write(serial_hander_t serial, const void *data, unsigned int size);
unsigned int serial_read(serial_hander_t serial, void *data, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif