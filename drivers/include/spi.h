#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_CPHA        (1 << 0)
#define SPI_CPOL        (1 << 1)

#define SPI_MSB         (0 << 2)
#define SPI_LSB         (1 << 2)

#define SPI_MASTER      (0 << 3)
#define SPI_SLAVE       (1 << 3)

#define SPI_4WIRE       (0 << 4)
#define SPI_3WIRE       (1 << 4)

#define SPI_DATA_8BIT   (0 << 5)
#define SPI_DATA_16BIT  (1 << 5)

#define SPI_MODE0       (0 | 0)
#define SPI_MODE1       (0 | SPI_CPHA)
#define SPI_MODE2       (SPI_CPOL | 0)
#define SPI_MODE3       (SPI_CPOL | SPI_CPHA)

typedef void* spi_hander_t;

spi_hander_t spi_open(unsigned int spi_id, unsigned int freq, unsigned int mode);

/**
 * @brief 控制spi传输一次数据
 * @param [inout] data 将data数据发出，同时接收数据到data
 * @param [in] size 传输数据的大小
 * @note 此接口会同时进行发送和接收，并将接收的数据覆盖发送的数据
 */
void spi_transfer(spi_hander_t spi, void *data, unsigned int size);
void spi_send(spi_hander_t spi, const void *data, unsigned int size);
void spi_recv(spi_hander_t spi, void *data, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif
