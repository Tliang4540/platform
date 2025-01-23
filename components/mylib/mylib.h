#ifndef MYLIB_H__
#define MYLIB_H__

#define UNUSED(X) ((void)X)
#define GET_BIT(v, b)   ((v) & (1 << (b)))

unsigned my_utoa(unsigned val, char *str, unsigned radix);

#endif
