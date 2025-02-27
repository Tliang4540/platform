#ifndef __WDT_H__
#define __WDT_H__

#ifdef __cplusplus
extern "C" {
#endif

void wdt_init(unsigned int period);
void wdt_reset(void);

#ifdef __cplusplus
}
#endif

#endif
