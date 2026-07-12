#ifndef __FOLLOW_LINE_H
#define __FOLLOW_LINE_H

void Follow_Line_Init(void);
void Follow_Start(void);
void Follow_Stop(void);
void Follow_Line_SetK(float kp, float ki, float kd);
void Follow_Line_Callback(void);

#endif
