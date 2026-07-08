#include "Buzzer.h"
#include "Buzzer_Test.h"
#include "Delay.h"

//测试Buzzer通断
void Buzzer_Test1(void)
{
    Buzzer_On();
    Delay_ms(1000);
    Buzzer_Off();
}

//测试Buzzer特定频率和时间通断
void Buzzer_Test2(void)
{
    Buzzer_On_ms(1000, 1000);
    Delay_ms(2000);
    Buzzer_On_ms(1000, 2000);
    Delay_ms(2000);
    Buzzer_On_ms(1000, 3000);
    Delay_ms(2000);
    Buzzer_On_ms(1000, 4000);
    Delay_ms(2000);
    Buzzer_On_ms(1000, 5000);
    Delay_ms(2000);
    Buzzer_On_ms(1000, 6000);
    Delay_ms(2000);
    Buzzer_On_ms(1000, 7000);
    while (1) {
    }
}