#include "PID.h"
#include "Delay.h"

void PID_Init(PIDController* pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->Goal = 0.0f;
    pid->Max_out = 1.0e30f;
    pid->Min_out = -1.0e30f;
    pid->Max_integral = 1.0e30f;
    pid->Min_integral = -1.0e30f;
    pid->is_first_compute = true;
    pid->last_compute_time = 0;
}

void PID_SetLimit(PIDController* pid, float Max_out, float Min_out, float Max_integral, float Min_integral)
{
    pid->Max_out = Max_out;
    pid->Min_out = Min_out;
    pid->Max_integral = Max_integral;
    pid->Min_integral = Min_integral;
}

void PID_SetGoal(PIDController* pid, float goal)
{
    pid->Goal = goal;
}

float PID_Compute(PIDController* pid, float error)
{
    uint64_t current_time = Get_us();
    if (pid->is_first_compute) {
        pid->last_compute_time = current_time;
        pid->last_error = error;
        pid->is_first_compute = false;
        return 0.0f;
    }

    float delta_time = (float)(current_time - pid->last_compute_time) * 1e-6f;
    float delta_error = error - pid->last_error;
    pid->integral += error * delta_time;
    // 积分限幅
    if (pid->integral > pid->Max_integral) {
        pid->integral = pid->Max_integral;
    } else if (pid->integral < pid->Min_integral) {
        pid->integral = pid->Min_integral;
    }

    float derivative = delta_error / delta_time;
    float output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;

    // 输出限幅
    if (output > pid->Max_out) {
        output = pid->Max_out;
    } else if (output < pid->Min_out) {
        output = pid->Min_out;
    }

    pid->last_error = error;
    pid->last_compute_time = current_time;

    return output;
}

//
//@简介: 计算PID控制器的输出，专门用于巡线控制，考虑了过零点清零积分和动态积分限幅
//@参数: pid - PID控制器结构体指针
//       FB - 当前反馈值（误差）
//@返回: PID控制器的输出值
// 注意：此函数包含了针对巡线控制的特殊处理，如过零点清零积分和动态积分限幅，以提高控制性能
float PID_Detect_Line_Compute(PIDController *pid, float error)
{
    uint64_t current_time = Get_us();
    if (pid->is_first_compute) {
        pid->last_compute_time = current_time;
        pid->last_error = error;
        pid->is_first_compute = false;
        return 0.0f;
    }

    // 添加死区，忽略微小偏差 
    //if (fabsf(FB) < 0.3f) {
    //    FB = 0.0f;
    //}
    // 检测过零点，清零积分 
    if ((pid->last_error > 0 && error < 0) ||
        (pid->last_error < 0 && error > 0)) {
        pid->integral = 0.0f;  // 偏差过零，清空积分
    }

    // 动态调整积分限幅 
    float integral_limit;
    if (fabsf(error) > 3.0f) {          // 大偏差时 
        integral_limit = 100.0f;
    } else if (fabsf(error) > 1.5f) {   // 中等偏差时 
        integral_limit = 50.0f;
    } else {                            // 小偏差时
        integral_limit = 20.0f;
    }

    float delta_time = (current_time - pid->last_compute_time) / 1000000.0f; // 转换为秒
    // 积分项，使用动态限幅
    pid->integral += (error + pid->last_error) * delta_time / 2.0f;
    if (pid->integral > integral_limit) {
        pid->integral = integral_limit;
    } else if (pid->integral < -integral_limit) {
        pid->integral = -integral_limit;
    }

    float derivative = 0.0f;
    // 微分项 
    if(delta_time > 1e-6f) {
        derivative = (error - pid->last_error) / delta_time;
    }
    

    // PID计算 
    float output = (pid->kp * error +
                   pid->ki * pid->integral +
                   pid->kd * derivative);

    // 更新上次偏差 / Update last error
    pid->last_error = error;
    pid->last_compute_time = current_time;

    if(output > pid->Max_out)
        output = pid->Max_out;
    else if(output < pid->Min_out)
        output = pid->Min_out;

    return output;
}

void PID_Reset(PIDController* pid)
{
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->is_first_compute = true;
}

void PID_SetK(PIDController* pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

