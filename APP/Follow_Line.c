#include "Follow_Line.h"
#include "Motor.h"
#include "Grayscale.h"
#include "PID.h"

static bool follow_line_cmd = false;
static uint16_t sensor_values[GRAYSCALE_SENSOR_CHANNELS];
static float sensor_weights[GRAYSCALE_SENSOR_CHANNELS] = 
    {-5.0f, -4.0f, -2.0f, -1.0f, 1.0f, 2.0f, 4.0f, 5.0f};
static PIDController PID_Line;
static float follow_line_speed = 300.0f; 
void Follow_Line_Init(void)
{
    follow_line_cmd = false;
    PID_Init(&PID_Line, 0.5f, 0.1f, 0.05f); // 初始化PID参数
    PID_SetLimit(&PID_Line, 360.0f, -360.0f, 200.0f, -200.0f); // 设置PID输出和积分限制
    follow_line_speed = 300.0f; // 设置巡线速度
}

void Follow_Start(void)
{
    PID_Reset(&PID_Line);
    follow_line_cmd = true;
    Motor_Start();
}

void Follow_Stop(void)
{
    follow_line_cmd = false;
    Motor_Stop();
}

void Follow_Line_SetK(float kp, float ki, float kd)
{
    PID_SetK(&PID_Line, kp, ki, kd);
}

static float calculate_error(uint16_t* sensor_values) 
{
    float weighted_sum = 0.0f;
    int active_sensors = 0;

    for (int i = 0; i < GRAYSCALE_SENSOR_CHANNELS; i++) {
        if (sensor_values[i] == 1) {
            weighted_sum += sensor_weights[i];
            active_sensors++;
        }
    }

    // 如果没有检测到线，返回上次偏差（丢线处理）
    if (active_sensors == 0) {
        return PID_Line.last_error * 1.2f; // 增大丢线时的偏差，增加回线力度
    }

    // 计算加权平均偏差
    float error = weighted_sum / active_sensors;
    return error;
}

void Follow_Line_Callback(void)
{
    if (follow_line_cmd)
    {
        Grayscale_Sensor_Read_All(sensor_values);
        float error = calculate_error(sensor_values);
        float output = PID_Detect_Line_Compute(&PID_Line, error);

        float left_motor_speed = follow_line_speed + output;
        float right_motor_speed = follow_line_speed - output;

        if (left_motor_speed < 0.0f) left_motor_speed = 0.0f;
        if (right_motor_speed < 0.0f) right_motor_speed = 0.0f;

        if (left_motor_speed > (follow_line_speed * 1.2f)) left_motor_speed = follow_line_speed * 1.2f;
        if (right_motor_speed > (follow_line_speed * 1.2f)) right_motor_speed = follow_line_speed * 1.2f;

        Motor_SetSpeed(MOTOR_LEFT, left_motor_speed);
        Motor_SetSpeed(MOTOR_RIGHT, right_motor_speed);
    }
}
