#include "ringbuffer.h"
#include <string.h>

void rb_init(rb_t *rb, void *buff, rb_size_t buffsize)
{
    rb->r = 0;
    rb->w = 0;
    rb->size = buffsize;
    rb->buff = buff;
}

rb_size_t rb_read(rb_t *rb, void *des, rb_size_t size)
{
    rb_size_t full;
    rb_size_t tocopy;

    if(size == 0)
        return 0;

    full = rb_get_full(rb);
    if(full == 0)
    {
        return 0;
    }
    size = full < size ? full : size;

    tocopy = rb->size - rb->r;
    tocopy = tocopy < size ? tocopy : size;
    memcpy(des, (unsigned char*)rb->buff + rb->r, tocopy);
    size -= tocopy;

    if(size)
    {
        memcpy((unsigned char*)des + tocopy, rb->buff, size);
        rb->r = size;
        tocopy += size;
    }
    else
    {
        rb->r += tocopy;
        if(rb->r >= rb->size)
            rb->r = 0;
    }

    return tocopy;
}

rb_size_t rb_write(rb_t *rb, const void *src, rb_size_t size)
{
    rb_size_t empty;
    rb_size_t tocopy;

    if(size == 0)
        return 0;
    // 不能完全填满，需要留一个空避免读写指针重合
    empty = rb->size - rb_get_full(rb) - 1;
    if(empty == 0)
    {
        return 0;
    }
    size = empty < size ? empty : size;

    tocopy = rb->size - rb->w;
    tocopy = tocopy < size ? tocopy : size;
    memcpy((unsigned char*)rb->buff + rb->w, src, tocopy);
    size -= tocopy;

    if(size)
    {
        memcpy(rb->buff, (unsigned char*)src + tocopy, size);
        rb->w = size;
        tocopy += size;
    }
    else
    {
        rb->w += tocopy;
        if(rb->w >= rb->size)
            rb->w = 0;
    }

    return tocopy;
}

rb_size_t rb_get_full(rb_t *rb)
{
    rb_size_t size;
    rb_size_t w;
    rb_size_t r;

    if(rb->r == rb->w)
        return 0;

    w = rb->w;
    r = rb->r;

    if(r < w)
        size = w - r;
    else 
        size = rb->size - (r - w);

    return size;
}
