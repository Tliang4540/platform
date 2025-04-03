#ifndef __TINYLIB_H__
#define __TINYLIB_H__

#define UNUSED(X) ((void)X)
#define GET_BIT(v, b)   ((v) & (1 << (b)))

unsigned int tiny_utoa(unsigned int val, char *str, unsigned int radix);

#endif
