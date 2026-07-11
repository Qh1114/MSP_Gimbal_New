#ifndef __CAMERA_RECEIVE_H
#define __CAMERA_RECEIVE_H

#include "ti_msp_dl_config.h"

#define CAMERA_HEADER        0x7A
#define CAMERA_TAIL          0x7B
#define CAMERA_MAX_FRAME_LEN 32
#define CAMERA_DATA_MAX_LEN  6

typedef struct {
    uint8_t funcCode;
    uint8_t dataLen;
} CameraFrameDef_t;

extern const CameraFrameDef_t CameraFrameDefTable[];
extern const uint8_t CameraFrameDefCount;

void Camera_Receive_Init(void);
void Camera_Receive_PutData(uint8_t funcCode, const uint8_t* data, uint8_t len);
uint8_t Camera_Receive_GetData(uint8_t funcCode, uint8_t* outBuf, uint8_t* outLen);
bool Camera_Get_Angle(float *up_angle,float *down_angle);

#endif