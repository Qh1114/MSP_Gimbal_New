#include "Camera_Receive.h"

const CameraFrameDef_t CameraFrameDefTable[] = {
    {0x10, 4},
};
const uint8_t CameraFrameDefCount = sizeof(CameraFrameDefTable) / sizeof(CameraFrameDefTable[0]);

typedef struct {
    uint8_t data[CAMERA_DATA_MAX_LEN];
    uint8_t len;
    uint8_t fresh;
} CameraDataSlot_t;

static CameraDataSlot_t dataSlots[8];

void Camera_Receive_Init(void)
{
    for (uint8_t i = 0; i < 8; i++) {
        dataSlots[i].fresh = 0;
    }
}

void Camera_Receive_PutData(uint8_t funcCode, const uint8_t* data, uint8_t len)
{
    for (uint8_t i = 0; i < CameraFrameDefCount; i++) {
        if (CameraFrameDefTable[i].funcCode == funcCode) {
            for (uint8_t j = 0; j < len; j++) {
                dataSlots[i].data[j] = data[j];
            }
            dataSlots[i].len = len;
            dataSlots[i].fresh = 1;
            break;
        }
    }
}

uint8_t Camera_Receive_GetData(uint8_t funcCode, uint8_t* outBuf, uint8_t* outLen)
{
    for (uint8_t i = 0; i < CameraFrameDefCount; i++) {
        if (CameraFrameDefTable[i].funcCode == funcCode) {
            if (!dataSlots[i].fresh) {
                return 0;
            }
            *outLen = dataSlots[i].len;
            for (uint8_t j = 0; j < dataSlots[i].len; j++) {
                outBuf[j] = dataSlots[i].data[j];
            }
            dataSlots[i].fresh = 0;
            return 1;
        }
    }
    return 0;
}

bool Camera_Get_Angle(float *up_angle,float *down_angle)
{
    uint8_t data[CAMERA_DATA_MAX_LEN];
    uint8_t len;
    if (Camera_Receive_GetData(0x10, data, &len) && len == 4) {
        int16_t rawAngle = (data[0] << 8) | data[1];
        *up_angle = rawAngle / 10.0f; 
        rawAngle = (data[2] << 8) | data[3];
        *down_angle = rawAngle / 10.0f;
        if (*up_angle > 360.0f  || *down_angle > 360.0f) {
            return false;
        }
        return true;
    }
    return false;
}