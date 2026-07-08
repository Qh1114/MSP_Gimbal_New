#include "Encoder_Test.h"
#include "ti_msp_dl_config.h"
#include "Encoder.h"
#include "Usart.h"
#include "tb6612.h"
#include "Delay.h"
void Encoder_Test1(void)
{
    int64_t count_l = 0, count_r = 0;
    Encoder_Init();
    while(1)
    {
        // int ena_r = DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
        // int enb_r = DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);
        // Uart0_Printf("%d,%d\n", ena_r, enb_r);
        Encoder_Count_Get(&count_l, &count_r);
        Uart0_Printf("%d,%d\n", (int32_t)count_l, (int32_t)count_r);
    }
}

void Encoder_Test2(void)
{
    int64_t count_l = 0, count_r = 0;
    Encoder_Init();
    while(1)
    {
        Encoder_Count_Get(&count_l, &count_r);
        float speed_l = Encoder_Speed_Get_LEFT();
        float speed_r = Encoder_Speed_Get_RIGHT();
        Uart0_Printf("%d,%d,%.2f,%.2f\n",
            (int32_t)count_l, (int32_t)count_r, speed_l, speed_r);
    }
}

void Encoder_Test3(void)
{
    int64_t count_l = 0, count_r = 0;
    Encoder_Init();
    TB6612_Init();
    TB6612_Forward(20.0f);
    while(1)
    {
        Encoder_Count_Get(&count_l, &count_r);
        float speed_l = Encoder_Speed_Get_LEFT();
        float speed_r = Encoder_Speed_Get_RIGHT();
        Uart0_Printf("%d,%d,%.2f, %.2f\n", (int32_t)count_l, (int32_t)count_r, speed_l, speed_r);
    }
}