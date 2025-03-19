#include "mylib.h"

unsigned my_utoa(unsigned val, char *str, unsigned radix)
{
    unsigned digit;
    char *start = str;

    if (radix == 16)
    {
        do
        {
            digit = val & 0xf;
            val >>= 4;
            *start++ = digit < 10 ? digit + '0' : digit + ('a' - 10);
        } while (val);
    }
    else
    {
        do
        {
            digit = val % radix;
            val /= radix;
            *start++ = digit + '0';
        } while (val);
    }
    
    unsigned len = start - str;
    start--;

    while (str < start)
    {
        char tmp = *str;
        *str++ = *start;
        *start-- = tmp;
    }

    return len;
}
