#ifndef __CLK_H__
#define __CLK_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum 
{
    CLK_LSC_EXTERNAL = 0,
    CLK_LSC_INTERNAL
}clk_lsc_t;

void clk_init(void);
void clk_lsc_init(clk_lsc_t lsc);

#ifdef __cplusplus
}
#endif

#endif
