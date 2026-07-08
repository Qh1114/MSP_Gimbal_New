#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "ti_msp_dl_config.h"
#include <stdbool.h>

typedef enum {
    ENA_L,
    ENB_L,
    ENA_R,
    ENB_R
} EncoderType;

void  Encoder_Init(void);
void  Encoder_Count_Get(int64_t* count_l, int64_t* count_r);
float Encoder_Speed_Get_LEFT(void);
float Encoder_Speed_Get_RIGHT(void);
bool  Encoder_New_Edge_LEFT(void);
bool  Encoder_New_Edge_RIGHT(void);
void  Encoder_Callback(EncoderType type, uint32_t timestamp_us);

#endif