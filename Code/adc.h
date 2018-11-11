#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

void adc_init(void);
void adc_start(uint8_t channel);

#endif // __ADC_H__
