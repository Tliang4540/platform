#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void* timer_handle_t;

timer_handle_t timer_open(unsigned int timerid, unsigned int freq, void (*hdr)(void));
void timer_start(timer_handle_t timer, unsigned int interval);
void timer_stop(timer_handle_t timer);
void timer_close(timer_handle_t timer);
unsigned int timer_read(timer_handle_t timer);

#ifdef __cplusplus
}
#endif

#endif
