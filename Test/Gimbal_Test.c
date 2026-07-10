#include "Gimbal_Send.h"
#include "Gimbal_Test.h"
#include "Delay.h"

void Gimbal_Test1(void) 
{
    Gimbal_Enable(GimbalID_Under);
    Gimbal_Enable(GimbalID_Up);

    Gimbal_SetMode(GimbalID_Under, GimbalMode_Speed);
    Gimbal_SetMode(GimbalID_Up, GimbalMode_Speed);

    Gimbal_SetSpeed(GimbalID_Under, 100);
    Gimbal_SetSpeed(GimbalID_Up, -100);

    Gimbal_SetMode(GimbalID_Under, GimbalMode_Multi_Turn_T);
    Gimbal_SetMode(GimbalID_Up, GimbalMode_Multi_Turn_T);

    Gimbal_SetAcceleration(GimbalID_Under, 1000);
    Gimbal_SetAcceleration(GimbalID_Up, 1000);

    Gimbal_MultiTurnMove(GimbalID_Under, 720.0f);
    Gimbal_MultiTurnMove(GimbalID_Up, -720.0f);
    Delay_ms(5000);

    Gimbal_SaveSettings(GimbalID_Under);
    Gimbal_SaveSettings(GimbalID_Up);
    
    Gimbal_Disable(GimbalID_Up);
    Gimbal_Disable(GimbalID_Under);
}

void Gimbal_Test2(void) 
{
    Delay_ms(2000);
    Gimbal_Enable(GimbalID_Up);   
    Delay_ms(2); 
    Gimbal_Enable(GimbalID_Under);
    Delay_ms(2);
    Gimbal_SetMode(GimbalID_Up, GimbalMode_Signal_Turn_T);
    Delay_ms(2);
    Gimbal_SetMode(GimbalID_Under, GimbalMode_Signal_Turn_T);
    Delay_ms(2);
    Gimbal_SetSpeed(GimbalID_Up, 100);
    Delay_ms(2);
    Gimbal_SetSpeed(GimbalID_Under, 100);
    Delay_ms(2);
    Gimbal_SingleTurnMove(GimbalID_Up, 0.0f);
    Delay_ms(2);
    Gimbal_SingleTurnMove(GimbalID_Under, 0.0f);

    uint32_t delay_time = 500;
    while(1)
    {
        for(uint16_t i = 0; i < 3600; i += 100) // 0.0 to 359.9 degrees
        {
            float position = i / 10.0f; // Convert to float with one decimal place
            Gimbal_SingleTurnMove(GimbalID_Up, position);
            Delay_us(delay_time);
            Gimbal_SingleTurnMove(GimbalID_Under, position);
            Delay_ms(30);
        }
    }
}