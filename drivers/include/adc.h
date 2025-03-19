#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void *adc_handle_t;

adc_handle_t adc_open(unsigned int adcid);
unsigned int adc_read(adc_handle_t adc);
void adc_set_channel(adc_handle_t adc, unsigned int channel);
void adc_close(adc_handle_t adc);

#ifdef __cplusplus
}
#endif

#endif
