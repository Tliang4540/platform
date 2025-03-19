#include <spi.h>
#include <audio/audio.h>
#include <pin.h>
#include <spiflash/spiflash.h>
#include <tinyos.h>
#include <log.h>
#include <string.h>
#include "audio_test.h"

struct audio_file_info
{
    char magic[4];
    unsigned short all_num;
    unsigned short voice_num;
    char version[8];
};

#define AUDIO_FILE_INFO_SIZE        64
#define AUDIO_FILE_START_ADDR       (1024 * 1024)

static struct audio_file_info file;
static unsigned int audio_file_list[256];

static spi_handle_t spi;
static spiflash_dev_t spiflash;
static unsigned int pwr_pin;
static void *fill_buffer;
static unsigned int fill_size;

static void audio_callback(void *buffer, unsigned int size)
{
    fill_buffer = buffer;
    fill_size = size;
}

void audio_test_init(void)
{
    pin_mode(17, PIN_MODE_FUNCTION_PP); //pwm
    pin_function(17, 1);
    audio_init(0, 3);
    pin_mode(35, PIN_MODE_OUTPUT_PP);   //pa power
    pwr_pin = 35;

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

    spi_init(&spi, 0, 52, 32000000, SPI_MODE0 | SPI_MSB | SPI_MASTER | SPI_4WIRE | SPI_DATA_8BIT);
    spiflash_init(&spiflash, &spi);
    spiflash_read(&spiflash, AUDIO_FILE_START_ADDR, &file, sizeof(file));
    LOG_I("all_num:%d", file.all_num);
    LOG_I("voice_num:%d", file.voice_num);
    LOG_I("version:%s", file.version);
    audio_file_list[0] = file.all_num * 4 + 64;
    spiflash_read(&spiflash, AUDIO_FILE_START_ADDR + 64, &audio_file_list[1], file.all_num * 4);
    for (unsigned int i = 0; i < 128; i++)
    {
        LOG_I("vocie %d addr:%x", i, audio_file_list[i]);
    }
}

void audio_test_play(unsigned int index)
{
    unsigned int size = audio_file_list[index + 1] - audio_file_list[index];
    unsigned int read_size;
    unsigned int addr = audio_file_list[index];

    audio_play_start(audio_callback);
    pin_write(pwr_pin, 1);

    while(size)
    {
        if (fill_size)
        {
            read_size = fill_size > size ? size : fill_size;
            spiflash_read(&spiflash, AUDIO_FILE_START_ADDR + addr, fill_buffer, read_size);
            for (unsigned int i = 0; i < read_size / 2; i++)
            {
                ((unsigned short *)fill_buffer)[i] = ((unsigned int)(((short*)fill_buffer)[i]) + 32768) >> 6;
            }
            for (unsigned int i = read_size / 2; i < fill_size / 2; i++)
            {
                ((unsigned short*)fill_buffer)[i] = 512;
            }
            size -= read_size;
            addr += read_size;
            fill_size = 0;
        }
        os_delay(1);
    }
    pin_write(pwr_pin, 0);
    audio_play_stop();
}
