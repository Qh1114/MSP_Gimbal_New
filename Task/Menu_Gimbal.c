#include "ti_msp_dl_config.h"
#include "Menu_Gimbal.h"
#include "Delay.h"
#include "Key.h"
#include "Usart.h"
#include "OLED.h"
#include "spi.h"
#include "Delay.h"
#include "Bat.h"
#include "Gimbal_Send.h"
#include <stdio.h>
#include "Camera_Receive.h"

static uint8_t Key = 0;
static char str[30];
void Menu_Gimbal_Pre(void)
{
    Key_Init();
    Uart_Init();
    SPI_Init();
    Bat_Init();
    OLED_Init();
    Camera_Receive_Init();

//------------------激光与编码器中断-------------------------//
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);
//------------------定时器中断------------------------------//
    NVIC_EnableIRQ(TIMER_10ms_INST_INT_IRQN);
	DL_Timer_startCounter(TIMER_10ms_INST);
//------------------定时器中断------------------------------//
    NVIC_EnableIRQ(TIMER_2ms_INST_INT_IRQN);
	DL_Timer_startCounter(TIMER_2ms_INST);
//------------------定时器中断------------------------------//
    NVIC_EnableIRQ(TIMER_100us_INST_INT_IRQN);
	DL_Timer_startCounter(TIMER_100us_INST);

    uint8_t menu_index = 0;
    char Oled_str[2][20] = {"Next", "Set Zero"};
    while(1) {
        Key = Key_Num_Get();
        switch(Key) 
        {
            case 1:
                menu_index = (menu_index + 1) % 2;
                break;
            case 2:
                menu_index = (menu_index + 1) % 2;
                break;
            case 3:
                switch(menu_index) 
                {
                    case 0:
                        Menu_Gimbal1();
                        break;
                    case 1:
                        Menu_Gimbal_SetZero();
                        break;
                }
                break;
            default:
                break;
        }

        PERIODIC_C(50);
        OLED_Clear();
        for(int i = 0; i < 2; i++) {
            if(i == menu_index) {
                OLED_ShowString(0, (i * 16) % 64, Oled_str[i], 16, 0);
            }else {
                OLED_ShowString(0, (i * 16) % 64, Oled_str[i], 16, 1);
            }
        }
        float battery_voltage = Bat_Read();
        sprintf(str, "Bat:%.2f", battery_voltage);
        OLED_ShowString(0, 32, str, 16, 1);
        OLED_Refresh();
    }
}

void Menu_Gimbal_SetZero(void)
{
    uint8_t menu_index = 0;
    char Oled_str[3][20] = {"<-", "Set Zero", "Off"};
    bool cmd = false;
    while(1) {
        Key = Key_Num_Get();
        switch(Key) 
        {
            case 1:
                menu_index = (menu_index + 1) % 3;
                break;
            case 2:
                menu_index = (menu_index + 2) % 3;
                break;
            case 3:
                switch(menu_index) 
                {
                    case 0:
                        return;
                    case 1:
                        Gimbal_SetZero(GimbalID_Up);
                        Delay_ms(1);
                        Gimbal_SetZero(GimbalID_Under);
                        Delay_ms(1);
                        Gimbal_SaveSettings(GimbalID_Up);
                        Delay_ms(2);
                        Gimbal_SaveSettings(GimbalID_Under);
                        Delay_ms(2);
                        break;
                    case 2:
                        cmd = !cmd;
                        if(cmd) {
                            Gimbal_Enable(GimbalID_Up);   
                            Delay_ms(2); 
                            Gimbal_Enable(GimbalID_Under);
                            Delay_ms(2);
                            Gimbal_SetMode(GimbalID_Up, GimbalMode_Signal_Turn_T);
                            Delay_ms(2);
                            Gimbal_SetMode(GimbalID_Under, GimbalMode_Signal_Turn_T);
                            Delay_ms(2);
                            Gimbal_SetSpeed(GimbalID_Up, 150);
                            Delay_ms(2);
                            Gimbal_SetSpeed(GimbalID_Under, 150);
                            Delay_ms(2);
                            Gimbal_SingleTurnMove(GimbalID_Up, 0.0f);
                            Delay_ms(2);
                            Gimbal_SingleTurnMove(GimbalID_Under, 0.0f);
                            sprintf(Oled_str[2], "On");
                        }else {
                            Gimbal_Disable(GimbalID_Up);
                            Delay_ms(1);
                            Gimbal_Disable(GimbalID_Under);
                            Delay_ms(1);
                            sprintf(Oled_str[2], "Off");
                        }
                        break;
                }
                break;
            default:
                break;
        }

        PERIODIC_C(50);
        OLED_Clear();
        for(int i = 0; i < 3; i++) {
            if(i == menu_index) {
                OLED_ShowString(0, (i * 16) % 64, Oled_str[i], 16, 0);
            }else {
                OLED_ShowString(0, (i * 16) % 64, Oled_str[i], 16, 1);
            }
        }
        OLED_Refresh();
    }
}

void Menu_Gimbal1(void)
{
    uint8_t menu_index = 0;
    char Oled_str[5][20] = {"<-", "Off"};
    bool cmd = false;
    float up_angle, down_angle;
    bool first_entry = true;
    while(1) {
        Key = Key_Num_Get();
        switch(Key) 
        {
            case 1:
                menu_index = (menu_index + 1) % 2;
                break;
            case 2:
                menu_index = (menu_index + 1) % 2;
                break;
            case 3:
                switch(menu_index) 
                {
                    case 0:
                        return;
                    case 1:
                        cmd = !cmd;
                        if(cmd) {
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
                            first_entry = true;
                            sprintf(Oled_str[1], "On");
                        }else {
                            Gimbal_Disable(GimbalID_Up);
                            Delay_ms(1);
                            Gimbal_Disable(GimbalID_Under);
                            Delay_ms(1);
                            sprintf(Oled_str[1], "Off");
                        }
                        break;
                }
                break;
            default:
                break;
        }

        if(cmd) {
            if(first_entry) {
                PERIODIC_C(1000);
                first_entry = false;
                uint8_t funcCode = 0x11; // 假设功能码为0x11
                uint8_t data[4] = {0x00, 0x00, 0x00, 0x01}; // 假设数据为4个字节的零值
                Uart_Camera_Send_Command(funcCode, data, 4);
            }
            Uart_Camera_ProcessDmaBuffer();
            if (Camera_Get_Angle(&up_angle, &down_angle)) {
                Gimbal_Send_UpDownAngle_DMA(up_angle, down_angle);
            }
        }
        PERIODIC_C(50);
        OLED_Clear();
        for(int i = 0; i < 2; i++) {
            if(i == menu_index) {
                OLED_ShowString(0, (i * 16) % 64, Oled_str[i], 16, 0);
            }else {
                OLED_ShowString(0, (i * 16) % 64, Oled_str[i], 16, 1);
            }
        }
        OLED_Refresh();
    }
}