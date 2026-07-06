#include "Maze.h"
#include "Maze_Test.h"
#include "Usart.h"
#include "Delay.h"
#include "IMU.h"
void Maze_Test1(void)
{
    IMU_init();
    Delay_ms(5000);
    Maze_Reset();
    Maze_SetStart(0, 2, MAZE_DIR_UP);
    Maze_SetGoal(3, 1);
    Maze_SetWallThresholdMm(220.0f);
    Maze_SetMoveProfile(38.0f);
    Maze_SelectAlgorithm(MAZE_ALGO_DFS);

    float yaw[4];
    Maze_GetYaw(yaw);
    while(1)
    {
        Uart0_Printf("DIR_UP Yaw: %.2f\r\n", yaw[MAZE_DIR_UP]);
        Uart0_Printf("DIR_RIGHT Yaw: %.2f\r\n", yaw[MAZE_DIR_RIGHT]);
        Uart0_Printf("DIR_DOWN Yaw: %.2f\r\n", yaw[MAZE_DIR_DOWN]);
        Uart0_Printf("DIR_LEFT Yaw: %.2f\r\n", yaw[MAZE_DIR_LEFT]);
        Delay_ms(1000);
    }
}