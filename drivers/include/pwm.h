#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void *pwm_hander_t;

pwm_hander_t pwm_open(unsigned int timerid, unsigned int freq, unsigned int duty);

#ifdef __cplusplus
}
#endif

#endif
