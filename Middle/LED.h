#ifndef __LED_H__
#define __LED_H__

#include "ti_msp_dl_config.h"
void LED_On(void);
void LED_Off(void);
void LED_On_ms(uint16_t ms);
void LED_tick(void);

#endif