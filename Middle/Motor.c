#include "Motor.h"
#include "ti_msp_dl_config.h"
#include "PID.h"
#include "Encoder.h"
#include "tb6612.h"
#include "Delay.h"
#include "Bat.h"

#define MOTOR_MIN_DUTY       3.0f   // 最小占空比，克服静摩擦
#define MIN_SPEED_THRESHOLD  60.0f  // 目标速度低于此值不强制最小占空比

#define GAIN_SWITCH_SPEED 100.0f  // 低于此速度切换低速增益

// 低速增益：Ki 大幅降低，防积分振荡
#define KP_LOW  0.01f
#define KI_LOW  0.1f
#define KD_LOW  0.00009f

// 正常增益
#define KP_NORMAL 0.01f
#define KI_NORMAL 0.9f
#define KD_NORMAL 0.00009f

PIDController pid_left;
PIDController pid_right;
volatile bool cmd_right_speed = false, cmd_left_speed = false;
volatile float Goal_Speed_Left = 0.0f, Goal_Speed_Right = 0.0f;
static bool low_speed_mode_l = false, low_speed_mode_r = false;

void Motor_Init(void)
{
    TB6612_Init();
    Encoder_Init();
    Bat_Init();
    PID_Init(&pid_left, 0.01f, 0.9f, 0.00009f);
    PID_Init(&pid_right, 0.01f, 0.9f, 0.00009f);
    PID_SetLimit(&pid_left, 7.0f, -7.0f, 5.0f, -5.0f);
    PID_SetLimit(&pid_right, 7.0f, -7.0f, 5.0f, -5.0f);
    Goal_Speed_Left = 0.0f;
    Goal_Speed_Right = 0.0f;
    cmd_right_speed = false;
    cmd_left_speed = false;
    low_speed_mode_l = false;
    low_speed_mode_r = false;
}

void Motor_SetSpeed(MotorType motor, float speed)
{
    if (motor == MOTOR_LEFT) {
        PID_SetGoal(&pid_left, speed);
        Goal_Speed_Left = speed;
        cmd_left_speed = true;
    } 
    if (motor == MOTOR_RIGHT) {
        PID_SetGoal(&pid_right, speed);
        Goal_Speed_Right = speed;
        cmd_right_speed = true;
    }
}

void Motor_Stop(void)
{
    cmd_left_speed = false;
    cmd_right_speed = false;
    TB6612_SetLeftMotor(0.0f);
    TB6612_SetRightMotor(0.0f);
}

void Motor_Start(void)
{
    cmd_left_speed = true;
    cmd_right_speed = true;
}

void Motor_SetK( float kp ,float ki, float kd)
{
    PID_SetK(&pid_left, kp, ki, kd);
    PID_SetK(&pid_right, kp, ki, kd);
}

void Motor_Callback(void)
{
    if(cmd_left_speed) {
        float abs_goal = Goal_Speed_Left > 0 ? Goal_Speed_Left : -Goal_Speed_Left;
        bool need_low = (abs_goal < GAIN_SWITCH_SPEED);
        if (need_low != low_speed_mode_l) {
            low_speed_mode_l = need_low;
            if (need_low) PID_SetK(&pid_left, KP_LOW, KI_LOW, KD_LOW);
            else          PID_SetK(&pid_left, KP_NORMAL, KI_NORMAL, KD_NORMAL);
            PID_Reset(&pid_left);
        }

        float Bat_Voltage, Output_l;
        float speed_l = Encoder_Speed_Get_LEFT();
        Bat_Voltage = Bat_Read();
        float pid_out = PID_Compute(&pid_left, (Goal_Speed_Left - speed_l));
        Output_l = pid_out / Bat_Voltage * 100.0f;
        if (Goal_Speed_Left > MIN_SPEED_THRESHOLD && Output_l < MOTOR_MIN_DUTY)       Output_l = MOTOR_MIN_DUTY;
        else if (Goal_Speed_Left < -MIN_SPEED_THRESHOLD && Output_l > -MOTOR_MIN_DUTY) Output_l = -MOTOR_MIN_DUTY;

        TB6612_SetLeftMotor(Output_l);
    }
    if(cmd_right_speed) {
        float abs_goal = Goal_Speed_Right > 0 ? Goal_Speed_Right : -Goal_Speed_Right;
        bool need_low = (abs_goal < GAIN_SWITCH_SPEED);
        if (need_low != low_speed_mode_r) {
            low_speed_mode_r = need_low;
            if (need_low) PID_SetK(&pid_right, KP_LOW, KI_LOW, KD_LOW);
            else          PID_SetK(&pid_right, KP_NORMAL, KI_NORMAL, KD_NORMAL);
            PID_Reset(&pid_right);
        }

        float Bat_Voltage, Output_r;
        float speed_r = Encoder_Speed_Get_RIGHT();
        Bat_Voltage = Bat_Read();
        float pid_out = PID_Compute(&pid_right, (Goal_Speed_Right - speed_r));
        Output_r = pid_out / Bat_Voltage * 100.0f;
        if (Goal_Speed_Right > MIN_SPEED_THRESHOLD && Output_r < MOTOR_MIN_DUTY)       Output_r = MOTOR_MIN_DUTY;
        else if (Goal_Speed_Right < -MIN_SPEED_THRESHOLD && Output_r > -MOTOR_MIN_DUTY) Output_r = -MOTOR_MIN_DUTY;

        TB6612_SetRightMotor(Output_r);
    }
}
