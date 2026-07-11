#ifndef __Gimbal_Send_H
#define __Gimbal_Send_H

#include "ti_msp_dl_config.h"

//云台模式枚举
typedef enum GimbalMode{
    GimbalMode_Speed = 0x00,            //云台速度模式
    GimbalMode_Multi_Turn_T = 0x01,     //云台多圈模式,带加减速
    GimbalMode_Signal_Turn_T = 0x02,    //云台单圈模式,带加减速
    GimbalMode_Multi_Turn = 0x03,       //云台多圈模式,不带加减速
    GimbalMode_Signal_Turn = 0x04,      //云台单圈模式,不带加减速
} GimbalMode_t;

typedef enum GimbalID{
    GimbalID_Under = 0x01,              //下云台
    GimbalID_Up = 0x02,                 //上云台
} GimbalID_t;

typedef enum GimbalTransmitState{
    GimbalTransmitState_Idle = 0,               //空闲状态
    GimbalTransmitState_Up_DMAMove,             //上云台DMA搬运中
    GimbalTransmitState_Up_WaitEndofTransmit,   //上云台发送完成
    GimbalTransmitState_Up_EndofTransmit,       //上云台发送完成
    GimbalTransmitState_Down_DMAMove,           //下云台DMA搬运中
    GimbalTransmitState_Down_WaitEndofTransmit, //下云台发送完成
} GimbalTransmitState_t;

void Gimbal_Enable(GimbalID_t GimbalID);
void Gimbal_Disable(GimbalID_t GimbalID);
void Gimbal_SetMode(GimbalID_t GimbalID, GimbalMode_t GimbalMode);
void Gimbal_SetZero(GimbalID_t GimbalID);
void Gimbal_SaveSettings(GimbalID_t GimbalID);
void Gimbal_SetSpeed(GimbalID_t GimbalID, int16_t speed);
void Gimbal_SetAcceleration(GimbalID_t GimbalID, uint16_t acceleration);
void Gimbal_MultiTurnMove(GimbalID_t GimbalID, float turn_angle);  
void Gimbal_SingleTurnMove(GimbalID_t GimbalID, float position);  
void Gimbal_Send_UpDownAngle_DMA(float up_angle, float down_angle);
void Gimbal_Transmit_Wait_DMA(void);
void Gimbal_Transmit_Wait_EndofTransmit(void);
void Gimbal_Send_Callback(void);

#endif
