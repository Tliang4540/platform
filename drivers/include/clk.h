#ifndef __CLK_H__
#define __CLK_H__

#ifdef __cplusplus
extern "C" {
#endif

void clk_init(void);
void clk_lse_init(void);
void clk_exti_sleep(void);

#ifdef __cplusplus
}
#endif

#endif
