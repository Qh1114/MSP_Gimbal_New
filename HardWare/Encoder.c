#include "Encoder.h"
#include "Delay.h"

#define PULSE_PER_REVOLUTION 265.0f
#define MAX_SPEED_DELTA      100.0f // 单次速度变化超过300°/s → 毛刺，用上次值
#define SPEED_FILTER_ALPHA   0.3f   // 低通滤波系数

volatile int64_t encoder_count_l = 0, encoder_count_r = 0;

static volatile uint32_t last_rising_l = 0, last_rising_r = 0;
static volatile uint32_t edge_dt_l    = 0, edge_dt_r    = 0;
static volatile int      direction_l  = 1, direction_r  = 1;  // 初始≠0, 避免 speed=0
static float filtered_speed_l = 0.0f, filtered_speed_r = 0.0f;
static float last_raw_l = 0.0f, last_raw_r = 0.0f;
static volatile bool new_edge_l = false, new_edge_r = false;

void Encoder_Init(void)
{
    encoder_count_l = 0; encoder_count_r = 0;
    last_rising_l = 0; last_rising_r = 0;
    edge_dt_l = 0; edge_dt_r = 0;
    direction_l = 1; direction_r = 1;
    filtered_speed_l = 0.0f; filtered_speed_r = 0.0f;
    last_raw_l = 0.0f; last_raw_r = 0.0f;
    new_edge_l = false; new_edge_r = false;
}

void Encoder_Count_Get(int64_t* count_l, int64_t* count_r)
{
    *count_l = encoder_count_l;
    *count_r = encoder_count_r;
}

float Encoder_Speed_Get_LEFT(void)
{
    uint32_t now, last, edge_dt;
    int dir;

    do {
        last    = last_rising_l;
        edge_dt = edge_dt_l;
        dir     = direction_l;
    } while (last != last_rising_l);

    now             = (uint32_t)Get_us();
    uint32_t elapsed = now - last;
    uint32_t dt      = (elapsed > edge_dt) ? elapsed : edge_dt;

    if (edge_dt == 0) return 0.0f;

    float raw = (float)dir * 360.0f * 1000000.0f
              / (PULSE_PER_REVOLUTION * (float)dt);
    if (raw > last_raw_l + MAX_SPEED_DELTA)      raw = last_raw_l + MAX_SPEED_DELTA;
    else if (raw < last_raw_l - MAX_SPEED_DELTA) raw = last_raw_l - MAX_SPEED_DELTA;
    last_raw_l = raw;
    filtered_speed_l += SPEED_FILTER_ALPHA * (raw - filtered_speed_l);
    return filtered_speed_l;
}

float Encoder_Speed_Get_RIGHT(void)
{
    uint32_t now, last, edge_dt;
    int dir;

    do {
        last    = last_rising_r;
        edge_dt = edge_dt_r;
        dir     = direction_r;
    } while (last != last_rising_r);

    now             = (uint32_t)Get_us();
    uint32_t elapsed = now - last;
    uint32_t dt      = (elapsed > edge_dt) ? elapsed : edge_dt;

    if (edge_dt == 0) return 0.0f;

    float raw = (float)dir * 360.0f * 1000000.0f
              / (PULSE_PER_REVOLUTION * (float)dt);
    if (raw > last_raw_r + MAX_SPEED_DELTA)      raw = last_raw_r + MAX_SPEED_DELTA;
    else if (raw < last_raw_r - MAX_SPEED_DELTA) raw = last_raw_r - MAX_SPEED_DELTA;
    last_raw_r = raw;
    filtered_speed_r += SPEED_FILTER_ALPHA * (raw - filtered_speed_r);
    return filtered_speed_r;
}

bool Encoder_New_Edge_LEFT(void)
{
    bool result = new_edge_l;
    new_edge_l = false;
    return result;
}

bool Encoder_New_Edge_RIGHT(void)
{
    bool result = new_edge_r;
    new_edge_r = false;
    return result;
}

void Encoder_Callback(EncoderType type, uint32_t timestamp_us)
{
    switch (type) {
        case ENA_L: {
            int ena = !!DL_GPIO_readPins(GPIOB, ENCODER_ENA_L_PIN);
            int enb = !!DL_GPIO_readPins(GPIOB, ENCODER_ENB_L_PIN);

            if (ena != enb) { encoder_count_l++; direction_l = 1; }
            else            { encoder_count_l--; direction_l = -1; }

            if (ena) {
                uint32_t new_dt = last_rising_l ? (timestamp_us - last_rising_l) : 0;
                last_rising_l = timestamp_us;
                edge_dt_l = new_dt;
                new_edge_l = true;
            }
            break;
        }
        case ENB_L: {
            int ena = !!DL_GPIO_readPins(GPIOB, ENCODER_ENA_L_PIN);
            int enb = !!DL_GPIO_readPins(GPIOB, ENCODER_ENB_L_PIN);
            if (enb == ena) { encoder_count_l++; direction_l = 1; }
            else            { encoder_count_l--; direction_l = -1; }
            break;
        }
        case ENA_R: {
            int ena = !!DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
            int enb = !!DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);

            if (ena == enb) { encoder_count_r++; direction_r = 1; }
            else            { encoder_count_r--; direction_r = -1; }

            if (ena) {
                uint32_t new_dt = last_rising_r ? (timestamp_us - last_rising_r) : 0;
                last_rising_r = timestamp_us;
                edge_dt_r = new_dt;
                new_edge_r = true;
            }
            break;
        }
        case ENB_R: {
            int ena = !!DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
            int enb = !!DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);
            if (enb != ena) { encoder_count_r++; direction_r = 1; }
            else            { encoder_count_r--; direction_r = -1; }
            break;
        }
    }
}