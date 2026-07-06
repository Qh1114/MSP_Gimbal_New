#ifndef __GRAYSCALE_SIGNAL_H
#define __GRAYSCALE_SIGNAL_H

#define Grayscale_Debounce_Count 5  // 灰度信号去抖动计数，连续多少次检测到黑线才认为真正进入下一格
                                    // 一次计数周期是 10ms（在 TIMER_10ms_INST_IRQHandler 中调用），所以这个值乘以 10ms 就是去抖动的时间窗口
bool Grayscale_Detect(void);
void Grayscale_Signal_Reset(void);
void Grayscale_Callback(void);

#endif 
