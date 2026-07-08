#include "ti_msp_dl_config.h"
#include "Buzzer.h"

#define CPU_FREQUENCY   CPUCLK_FREQ
#define Clock_Division  8
#define Clock_Prescaler 10
#define Clock_Tick      2           // 2ms

volatile bool buzzer_cmd = false;
volatile uint32_t buzzer_on_counter = 0;

void Buzzer_On(void)
{
    DL_Timer_startCounter(PWM_BUZZER_INST);
}

void Buzzer_Off(void)
{
    DL_Timer_stopCounter(PWM_BUZZER_INST);
}

void Buzzer_On_ms(uint16_t ms, float frequency)
{
    DL_TimerA_setCaptureCompareValue(PWM_BUZZER_INST, (uint16_t)(CPU_FREQUENCY / Clock_Prescaler / Clock_Division / frequency),GPIO_PWM_BUZZER_C0_IDX);
    buzzer_on_counter = ms / Clock_Tick;
    buzzer_cmd = true;
    Buzzer_On();
}

void Buzzer_tick(void)
{
    if (buzzer_cmd) {
        if (buzzer_on_counter > 0) {
            buzzer_on_counter--;
        } else {
            buzzer_cmd = false;
            Buzzer_Off();
        }
    }
}