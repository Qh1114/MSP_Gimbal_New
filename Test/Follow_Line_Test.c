#include "Follow_Line.h"
#include "Motor.h"
#include "OLED.h"
#include "spi.h"
#include "Key.h"
#include <stdio.h>
#include "Delay.h"

//测试循迹功能
void Follow_Line_Test1(void)
{
    SPI_Init();
    OLED_Init();
    Motor_Init();
    Follow_Line_Init();
    char buffer[20];
    OLED_Clear();
    sprintf(buffer, "Follow Line: OFF");
    OLED_ShowString(0, 0, buffer, 16, 1);
    OLED_Refresh();
    while(1) 
    {
        uint8_t key = Key_Num_Get();
        if (key == 1)
        {
            Follow_Start();
            OLED_Clear();
            sprintf(buffer, "Follow Line: ON");
            OLED_ShowString(0, 0, buffer, 16, 1);
            OLED_Refresh();
        }
        else if (key == 2)
        {
            Follow_Stop();
            OLED_Clear();
            sprintf(buffer, "Follow Line: OFF");
            OLED_ShowString(0, 0, buffer, 16, 1);
            OLED_Refresh();
        }
    }
}

//测试循迹功能，带PID参数调节
void Follow_Line_Test2(void)
{
    SPI_Init();
    OLED_Init();
    Motor_Init();
    Follow_Line_Init();

    char buffer[20];
    bool follo_line_cmd = false;
    uint8_t key = 0;
    uint8_t menu_index = 0;
    char menu[4][20] = {"OFF", "Kp:", "Ki:", "Kd:"};
    float kp = 0.5;
    float ki = 0.0;
    float kd = 0.0;
    float kp_step = 1;
    float ki_step = 0.1;
    float kd_step = 0.1;
    bool OLED_Refresh_Flag = true;
    Follow_Line_SetK(kp, ki, kd);
    while(1)
    {
        key = Key_Num_Get();
        switch(key)
        {
            case 1:
                menu_index = (menu_index + 1) % 4;
                break;
            case 2:
                menu_index = (menu_index + 3) % 4;
                break;
            case 3:
                OLED_Refresh_Flag = true;
                switch(menu_index)
                {
                    case 0:
                        follo_line_cmd = !follo_line_cmd;
                        if (follo_line_cmd)
                        {
                            Follow_Line_SetK(kp, ki, kd);
                            Follow_Start();        
                            sprintf(menu[0], "ON");                    
                        }
                        else
                        {
                            Follow_Stop();
                            Motor_Stop();
                            sprintf(menu[0], "OFF");
                        }
                        break;
                    case 1:
                        kp += kp_step;
                        break;
                    case 2:
                        ki += ki_step;
                        break;
                    case 3:
                        kd += kd_step;
                        break;
                }
                break;
            case 4:
                OLED_Refresh_Flag = true;
                switch(menu_index)
                {
                    case 1:
                        if (kp > 0.1)
                            kp -= kp_step;
                        break;
                    case 2:
                        if (ki > 0.01)
                            ki -= ki_step;
                        break;
                    case 3:
                        if (kd > 0.01)
                            kd -= kd_step;
                        break;
                }
                break;
            default:
                break;
        }
        
        if (OLED_Refresh_Flag)
        {
            OLED_Clear();
            for (int i = 0; i < 4; i++)
            {
                if (i == menu_index)
                {
                    if(i == 1)
                        sprintf(buffer, ">%s %.2f", menu[i], kp);
                    else if(i == 2)
                        sprintf(buffer, ">%s %.2f", menu[i], ki);
                    else if(i == 3)
                        sprintf(buffer, ">%s %.2f", menu[i], kd);
                    else
                        sprintf(buffer, ">%s", menu[i]);
                }
                else
                {
                    if(i == 1)
                        sprintf(buffer, " %s %.2f", menu[i], kp);
                    else if(i == 2)
                        sprintf(buffer, " %s %.2f", menu[i], ki);
                    else if(i == 3)
                        sprintf(buffer, " %s %.2f", menu[i], kd);
                    else
                        sprintf(buffer, " %s", menu[i]);
                }
                OLED_ShowString(0, i * 16, buffer, 16, 1);
            }
            OLED_Refresh();
        }
    }

}