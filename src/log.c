#include <log.h>
#include <serial.h>
#include <mylib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#if LOG_LEVEL

static serial_hander_t m_serial;
static char log_buf[128];    //防止任务栈溢出, 使用静态变量

void log_printf(const char *format, ...)
{
    unsigned leng = 0;
    char ch;
    va_list args;

    va_start(args, format);

    while(*format)
    {
        // 防止溢出
        // if (leng > (sizeof(log_buf) / 2))
        // {
        //     serial_write(m_serial, log_buf, leng);
        //     leng = 0;
        // }

        ch = *format;
        format++;
        if(ch != '%')
        {
            log_buf[leng++] = ch;
            continue;
        }

        ch = *format;
        format++;

        switch (ch)
        {
        case 'd':
        {
            int tmp = va_arg(args, int);
            if(tmp < 0)
            {
                log_buf[leng++] = '-';
                tmp = -tmp;
            }
            leng += my_utoa(tmp, &log_buf[leng], 10);
            break;
        }
        case 'x':
        {
            unsigned int tmp = va_arg(args, unsigned int);
            unsigned int mask = 0xfffffff0;
            unsigned int count = 0;
            while(tmp & mask)
            {
                mask <<= 4;
                count++;
            }
            if((count & 1) == 0)
                log_buf[leng++] = '0';
            leng += my_utoa(tmp, &log_buf[leng], 16);
            break;
        }
        case 's':
        {
            char *str = va_arg(args, char*);
            while (*str)
            {
                log_buf[leng++] = *str++;
            }
            break;
        }
        default:
            log_buf[leng++] = '%';
            format--;
            break;
        }
    }

    va_end(args);
    serial_write(m_serial, log_buf, leng);
}

void log_dump(const void *data, unsigned leng)
{
    unsigned i;
    unsigned buf_index = 0;
    unsigned char *dump = (unsigned char*)data;

    for(i = 0; i < leng; i++)
    {
        log_buf[buf_index++] = (*dump >> 4) < 10 ? '0' + (*dump >> 4) : 'a' + (*dump >> 4) - 10;
        log_buf[buf_index++] = (*dump & 15) < 10 ? '0' + (*dump & 15) : 'a' + (*dump & 15) - 10;

        if(((i & 0x07) == 7) || ((i + 1) == leng))
            log_buf[buf_index] = '\n';
        else
            log_buf[buf_index] = ' ';

        buf_index++;
        dump++;
        // 防止溢出, 人为控制不要溢出
        // if (bug_index > (sizeof(log_buf) / 2))
        // {
        //     serial_write(m_serial, log_buf, bug_index);
        //     leng = 0;
        // }
    }
    serial_write(m_serial, log_buf, buf_index);
}

#ifndef NDEBUG
void log_assert(const char *ex_str, const char *func, unsigned int line)
{
    log_printf("%s:%d:(%s)\n", func, line, ex_str);
    while(1);
}
#endif

void log_init(unsigned serial_id, unsigned baud)
{
    m_serial = serial_open(serial_id, baud);
}

#endif
