#include "LED.h"

#define LED_Tick 2         // 2ms

volatile bool led_cmd = false;
volatile uint32_t led_on_counter = 0;
void LED_On(void)
{
    DL_GPIO_clearPins(LED_PORT, LED_LED_1_PIN);
}

void LED_Off(void)
{
    DL_GPIO_setPins(LED_PORT, LED_LED_1_PIN);
}

void LED_On_ms(uint16_t ms)
{
    led_on_counter = ms / LED_Tick;
    led_cmd = true;
    DL_GPIO_clearPins(LED_PORT, LED_LED_1_PIN);
}

void LED_tick(void)
{
    if (led_cmd) {
        if (led_on_counter > 0) {
            led_on_counter--;
        } else {
            led_cmd = false;
            DL_GPIO_setPins(LED_PORT, LED_LED_1_PIN);
        }
    }
}