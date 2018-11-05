#ifndef __HC595_H__
#define __HC595_H__

#include <stdint.h>

void hc595_init(void);
void hc595_write(uint8_t *byte, uint8_t size);

#endif // __HC595_H__
