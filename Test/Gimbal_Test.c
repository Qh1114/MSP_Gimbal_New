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

