#include "Gimbal_Send.h"
#include "Gimbal_Test.h"
#include "Delay.h"
#include "Usart.h"
#include "Camera_Receive.h"

//串口测试
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

//云台功能测试
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

//dma串口测试
void Gimbal_Test3(void) 
{
    uint8_t command[4] = {0x01, 0x01, 0x00, 0x64}; 
    Uart_Gimbal_Send_Command_DMA(command, 4);
    while(1)
    {
    }
}

//dma发送两个角度，带有延时测试
void Gimbal_Test4(void) 
{
    float up_angle = 90.0f; // 上云台目标角度
    float down_angle = 45.0f; // 下云台目标角度
    while(1)
    {
        Gimbal_Send_UpDownAngle_DMA(up_angle, down_angle);
        Delay_ms(30); 
    }
}

//测试从相机接收数据，并通过串口发送到云台
void Gimbal_Test5(void) 
{
    Camera_Receive_Init();
    float up_angle, down_angle;
    while (1) {
        Uart_Camera_ProcessDmaBuffer();
        if (Camera_Get_Angle(&up_angle, &down_angle)) {
            Gimbal_Send_UpDownAngle_DMA(up_angle, down_angle);
        }
    }
}

//测试从相机接收数据，并通过串口发送到云台，对其进行真实的角度控制
void Gimbal_Test6(void) 
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
    Camera_Receive_Init();
    float up_angle, down_angle;
    while (1) {
        Uart_Camera_ProcessDmaBuffer();
        if (Camera_Get_Angle(&up_angle, &down_angle)) {
            Gimbal_Send_UpDownAngle_DMA(up_angle, down_angle);
        }
    }
}