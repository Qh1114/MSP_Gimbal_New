#include "LED.h"
#include "LED_Test.h"
#include "Delay.h"
void LED_Test1(void)
{
    LED_On_ms(1000);
    Delay_ms(2000);
    LED_On_ms(2000);
    while (1) {
    }
}