#include "Camera_Test.h"
#include "Camera_Receive.h"
#include "Usart.h"

void Camera_Test1(void)
{
    Camera_Receive_Init();
    float up_angle, down_angle;
    while (1) {
        Uart_Camera_ProcessDmaBuffer();
        if (Camera_Get_Angle(&up_angle, &down_angle)) {
            Uart_Camera_Printf("Up Angle: %.2f, Down Angle: %.2f\n", up_angle, down_angle);
        }
    }
}
