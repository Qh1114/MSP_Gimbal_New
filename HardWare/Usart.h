#ifndef __USART_H__
#define __USART_H__

#include "ti_msp_dl_config.h"
void Uart_Init(void);
void Uart0_Send_Char(char ch);
void Uart0_Send_String(char* str);
void Uart0_Printf(const char* format, ...);
uint8_t UART_ReadCommand(uint8_t* command);

void Uart_Gimbal_Send_String(char* str);
void Uart_Gimbal_Send_Command(uint8_t* command, uint8_t length);

void Uart_Camera_Send_Char(char ch);
void Uart_Camera_Send_String(char* str);
void Uart_Camera_Printf(const char* format, ...);
void Uart_Camera_Get_Command_Count(uint32_t* count, uint32_t* index);
    
#endif
