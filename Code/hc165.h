#ifndef __HC165_H__
#define __HC165_H__

#include <stdint.h>

void hc165_init(void);
void hc165_read(uint8_t *byte, uint8_t size);

#endif // __HC165_H__
