#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "ti_msp_dl_config.h"

void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_On_ms(uint16_t ms, float frequency);
void Buzzer_tick(void);

#endif
