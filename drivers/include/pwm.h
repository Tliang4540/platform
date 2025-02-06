#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void *pwm_hander_t;

pwm_hander_t pwm_open(unsigned int pwmid);
void pwm_set(pwm_hander_t pwm, unsigned int channel, unsigned int period, unsigned int pulse);
void pwm_enable(pwm_hander_t pwm, unsigned int channel);
void pwm_disable(pwm_hander_t pwm, unsigned int channel);
void pwm_close(pwm_hander_t pwm);

#ifdef __cplusplus
}
#endif

#endif
