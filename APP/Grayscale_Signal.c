#include "Grayscale.h"
#include "Grayscale_Signal.h"

static uint8_t debounceCounter = 0; // 去抖动计数器
static bool lastGrayState = false; // 上一次的灰度状态
volatile static bool Detected = false; // 是否检测到格线

bool Grayscale_Detect(void)
{
    if (Detected)
    {
        Detected = false; // 重置检测状态，等待下一次检测
        return true;
    }

    return false;
}

void Grayscale_Signal_Reset(void)
{
    Detected = false;
    debounceCounter = 0;
    lastGrayState = false;
}

void Grayscale_Callback(void)
{
    if(Detected) return; // 已经检测到格线，等待上层处理后重置
        
    bool signal_1 = Grayscale_Sensor_Read_Single(0);
    bool signal_2 = Grayscale_Sensor_Read_Single(1);
    bool signal_3 = Grayscale_Sensor_Read_Single(6);
    bool signal_4 = Grayscale_Sensor_Read_Single(7);
    if (signal_1 || signal_2 || signal_3 || signal_4)
    {
        if (!lastGrayState)
        {
            debounceCounter++;
            if (debounceCounter >= Grayscale_Debounce_Count)
            {
                Detected = true; // 确认进入下一格
                lastGrayState = true;
                debounceCounter = 0; // 重置计数器
            }
        }
    }
    else
    {
        lastGrayState = false;
        debounceCounter = 0; // 重置计数器
    }
}