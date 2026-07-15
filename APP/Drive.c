#include "Drive.h"
#include "Motor.h"
#include "IMU.h"
#include "PID.h"
#include "tb6612.h"

bool cmd_straight_drive = false,cmd_angle_drive = false,cmd_angle_drive_during_motion = false;
PIDController pid_straight_drive,pid_angle_drive,pid_angle_drive_during_motion;
float Goal_Straight_Angle = 0.0f, Goal_Turn_Angle = 0.0f, Goal_Turn_Angle_During_Motion = 0.0f;
float speed = 0.0f, speed_during_motion = 0.0f;

//@简介：初始化驱动模块
//@参数：无
void Drive_Init(void)
{
    Motor_Init();
    IMU_init();
    PID_Init(&pid_straight_drive, 25.0f, 0.0f, 0.05f);
    PID_SetLimit(&pid_straight_drive, 1000.0f, -1000.0f, 200.0f, -200.0f);
    PID_Init(&pid_angle_drive, 20.0f, 0.0f, 0.0f);
    PID_SetLimit(&pid_angle_drive, 1000.0f, -1000.0f, 200.0f, -200.0f);
    PID_Init(&pid_angle_drive_during_motion, 20.0f, 0.0f, 0.0f);
    PID_SetLimit(&pid_angle_drive_during_motion, 1000.0f, -1000.0f, 200.0f, -200.0f);
    Goal_Straight_Angle = 0.0f;
    Goal_Turn_Angle = 0.0f;
    Goal_Turn_Angle_During_Motion = 0.0f;
    cmd_straight_drive = false;
    cmd_angle_drive = false;
    cmd_angle_drive_during_motion = false;
}

//@简介：直线行驶
//@参数：speed_in:行驶速度，angle:目标角度
//@返回值：无
void Drive_Straight(float speed_in, float angle)
{
    PID_SetGoal(&pid_straight_drive, angle);
    Goal_Straight_Angle = angle;
    speed = speed_in;
    cmd_straight_drive = true;
    cmd_angle_drive = false;
    cmd_angle_drive_during_motion = false;
}

//@简介：设置直线行驶PID参数
//@参数：kp:比例系数, ki:积分系数, kd:微分系数
//@返回值：无
void Drive_Straight_SetK(float kp, float ki, float kd)
{
    PID_SetK(&pid_straight_drive, kp, ki, kd);
}

//@简介：原地转向行驶
//@参数：angle:目标角度
//@返回值：无
void Drive_Turn(float angle)
{
    PID_SetGoal(&pid_angle_drive, angle);
    Goal_Turn_Angle = angle;
    cmd_angle_drive = true;
    cmd_straight_drive = false;
    cmd_angle_drive_during_motion = false;
}

//@简介：设置原地转向行驶PID参数
//@参数：kp:比例系数, ki:积分系数, kd:微分系数
//@返回值：无
void Drive_Turn_SetK(float kp, float ki, float kd)
{
    PID_SetK(&pid_angle_drive, kp, ki, kd);
}

//@简介：行驶中转向行驶
//@参数：angle:目标角度, speed_in:行驶速度
//返回值：无
void Drive_Turn_During_Motion(float angle, float speed_in)
{
    PID_SetGoal(&pid_angle_drive_during_motion, angle);
    Goal_Turn_Angle_During_Motion = angle;
    speed_during_motion = speed_in;
    cmd_angle_drive_during_motion = true;
    cmd_straight_drive = false;
    cmd_angle_drive = false;
}

//@简介：设置行驶中转向行驶PID参数
//@参数：kp:比例系数, ki:积分系数, kd:微分系数
//@返回值：无
void Drive_Turn_During_Motion_SetK(float kp, float ki, float kd)
{
    PID_SetK(&pid_angle_drive_during_motion, kp, ki, kd);
}

//@简介：停止所有行驶
//@参数：无
//@返回值：无
void Drive_Stop(void)
{
    cmd_straight_drive = false;
    cmd_angle_drive = false;
    cmd_angle_drive_during_motion = false;
    Motor_Stop();
    Motor_SetSpeed(MOTOR_LEFT, 0.0f);
    Motor_SetSpeed(MOTOR_RIGHT, 0.0f);
}


static float CalculateAngleError(float target_angle, float current_angle)
{
    float error = target_angle - current_angle;
    if (error > 180.0f) {
        error -= 360.0f;
    } else if (error < -180.0f) {
        error += 360.0f;
    }
    return error;
}

void Drive_Callback(void)
{
    if(cmd_angle_drive)
    {
        float current_angle = IMU_GetYaw();
        float angle_error = CalculateAngleError(Goal_Turn_Angle, current_angle);
        float correction = PID_Compute(&pid_angle_drive, angle_error);
        Motor_SetSpeed(MOTOR_LEFT, -correction);
        Motor_SetSpeed(MOTOR_RIGHT, correction);
    }
    else if(cmd_straight_drive)
    {
        float current_angle = IMU_GetYaw();
        float angle_error = CalculateAngleError(Goal_Straight_Angle, current_angle);
        float correction = PID_Compute(&pid_straight_drive, angle_error);
        Motor_SetSpeed(MOTOR_LEFT, speed - correction);
        Motor_SetSpeed(MOTOR_RIGHT, speed + correction);
    }
    else if(cmd_angle_drive_during_motion)
    {
        float current_angle = IMU_GetYaw();
        float angle_error = CalculateAngleError(Goal_Turn_Angle_During_Motion, current_angle);
        float correction = PID_Compute(&pid_angle_drive_during_motion, angle_error);
        Motor_SetSpeed(MOTOR_LEFT, speed_during_motion - correction);
        Motor_SetSpeed(MOTOR_RIGHT, speed_during_motion + correction);
    }
}
