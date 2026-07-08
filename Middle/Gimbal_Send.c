#include "Gimbal_Send.h"
#include "Usart.h"

static uint8_t GimbalCommandBuffer[10];

void Gimbal_Enable(GimbalID_t GimbalID) 
{
    GimbalCommandBuffer[0] = GimbalID;
    GimbalCommandBuffer[1] = 0x06; 
    Uart_Gimbal_Send_Command(GimbalCommandBuffer, 2);
}

void Gimbal_Disable(GimbalID_t GimbalID) 
{
    GimbalCommandBuffer[0] = GimbalID;
    GimbalCommandBuffer[1] = 0x05;
    Uart_Gimbal_Send_Command(GimbalCommandBuffer, 2);
}

void Gimbal_SetMode(GimbalID_t GimbalID, GimbalMode_t GimbalMode) 
{
    GimbalCommandBuffer[0] = GimbalID;
    GimbalCommandBuffer[1] = 0x00;
    GimbalCommandBuffer[2] = 0x00; 
    GimbalCommandBuffer[3] = GimbalMode; 
    Uart_Gimbal_Send_Command(GimbalCommandBuffer, 4);
}

//@简介：设置云台加速度
//@参数：GimbalID:云台ID
//@参数：acceleration:加速度值，单位为rpm/(s^2)
void Gimbal_SetAcceleration(GimbalID_t GimbalID, uint16_t acceleration) 
{
    GimbalCommandBuffer[0] = GimbalID;
    GimbalCommandBuffer[1] = 0x07; 
    uint8_t accLow = acceleration & 0xFF;    
    uint8_t accHigh = (acceleration >> 8) & 0xFF;
    GimbalCommandBuffer[2] = accHigh;
    GimbalCommandBuffer[3] = accLow;
    Uart_Gimbal_Send_Command(GimbalCommandBuffer, 4);
}

void Gimbal_SaveSettings(GimbalID_t GimbalID) 
{
    GimbalCommandBuffer[0] = GimbalID;
    GimbalCommandBuffer[1] = 0x08; 
    Uart_Gimbal_Send_Command(GimbalCommandBuffer, 2);
}

//@简介：设置云台速度
//@参数：GimbalID:云台ID
//@参数：speed:速度值，范围-1000-1000rpm
void Gimbal_SetSpeed(GimbalID_t GimbalID, int16_t speed) 
{
    GimbalCommandBuffer[0] = GimbalID;
    GimbalCommandBuffer[1] = 0x01; 
    uint8_t speedLow = speed & 0xFF;    
    uint8_t speedHigh = (speed >> 8) & 0xFF;
    GimbalCommandBuffer[2] = speedHigh;
    GimbalCommandBuffer[3] = speedLow;
    Uart_Gimbal_Send_Command(GimbalCommandBuffer, 4);
}

//@简介：云台多圈模式移动
//@参数：GimbalID:云台ID
//@参数：turn_angle:目标转角，单位为度
void Gimbal_MultiTurnMove(GimbalID_t GimbalID, float turn_angle) 
{
    GimbalCommandBuffer[0] = GimbalID;
    GimbalCommandBuffer[1] = 0x02; 
    int32_t angleValue = (int32_t)(turn_angle * 10); 
    uint8_t angleByte1 = (angleValue >> 24) & 0xFF;
    uint8_t angleByte2 = (angleValue >> 16) & 0xFF;
    uint8_t angleByte3 = (angleValue >> 8) & 0xFF;
    uint8_t angleByte4 = angleValue & 0xFF;
    GimbalCommandBuffer[2] = angleByte1;
    GimbalCommandBuffer[3] = angleByte2;
    GimbalCommandBuffer[4] = angleByte3;
    GimbalCommandBuffer[5] = angleByte4;
    Uart_Gimbal_Send_Command(GimbalCommandBuffer, 6);
}

//@简介：云台单圈模式移动
//@参数：GimbalID:云台ID
//@参数：position:目标位置，单位为度
//@注意：position的范围为0-359.9度
void Gimbal_SingleTurnMove(GimbalID_t GimbalID, uint16_t position)
{
    GimbalCommandBuffer[0] = GimbalID;
    GimbalCommandBuffer[1] = 0x03; 
    uint8_t positionLow = position & 0xFF;    
    uint8_t positionHigh = (position >> 8) & 0xFF;
    GimbalCommandBuffer[2] = positionHigh;
    GimbalCommandBuffer[3] = positionLow;
    Uart_Gimbal_Send_Command(GimbalCommandBuffer, 4);
}