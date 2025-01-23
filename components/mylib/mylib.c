#include "mylib.h"

unsigned my_utoa(unsigned val, char *str, unsigned radix)
{
    unsigned leng = 0;
    unsigned digit;

    do
    {
        digit = val % radix;
        val /= radix;

        str[leng++] = digit < 10 ? '0' + digit : 'a' + digit - 10;
    } while (val);
    
    for (unsigned i = 0, n = leng - 1; i < n; i++, n--)
    {
        char tmp = str[i];
        str[i] = str[n];
        str[n] = tmp;
    }

    return leng;
}
