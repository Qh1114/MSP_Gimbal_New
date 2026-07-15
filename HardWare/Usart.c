#include "ti_msp_dl_config.h"
#include "Usart.h"
#include "Delay.h"
#include "string.h"
#include "stdarg.h"
#include <stdio.h>
#include "Camera_Receive.h"
#include "Gimbal_Send.h"

#define BufferSize 256          //缓冲区大小
#define CommandMiniSize 4       //最小命令长度
#define CommandStartByte 0xAA   //命令起始字节
#define CommandEndByte 0xBB     //命令结束字节

//----------------云台串口----------------//
#define Gimbal_StartByte 0x7A
#define Gimbal_EndByte 0x7B
//---------------------------------------//
//----------------相机串口----------------//
#define Camera_StartByte 0x7A
#define Camera_EndByte 0x7B
//---------------------------------------//

uint8_t ReadIndex = 0;
uint8_t WriteIndex = 0;
uint8_t Buffer[BufferSize];
uint8_t data;

static uint8_t GimbalCommandBuffer[10];

uint8_t CameraCommandBuffer[BufferSize];
uint32_t CommandCount = 0;
static uint32_t lastCameraDmaPos = 0;

// 相机帧解析状态机
static uint8_t cameraState = 0;
static uint8_t cameraRxBuffer[CAMERA_MAX_FRAME_LEN];
static uint8_t cameraRxIndex;
static uint8_t cameraTotalLen;

#define CAMERA_STATE_IDLE       0
#define CAMERA_STATE_GOT_HEADER 1
#define CAMERA_STATE_RECEIVING  2

void Uart_Init(void)
{
    NVIC_ClearPendingIRQ(UART_INST_INT_IRQN);
    (void)DL_UART_Main_receiveData(UART_INST);
    NVIC_EnableIRQ(UART_INST_INT_IRQN);

    NVIC_ClearPendingIRQ(UART_Gimbal_INST_INT_IRQN);
    (void)DL_UART_Main_receiveData(UART_Gimbal_INST);
    NVIC_EnableIRQ(UART_Gimbal_INST_INT_IRQN);
    
    NVIC_ClearPendingIRQ(UART_CAMERA_INST_INT_IRQN);
    (void)DL_UART_Main_receiveData(UART_CAMERA_INST);
    NVIC_EnableIRQ(UART_CAMERA_INST_INT_IRQN);

    NVIC_ClearPendingIRQ(UART_WIRELESS_INST_INT_IRQN);
    (void)DL_UART_Main_receiveData(UART_WIRELESS_INST);
    NVIC_EnableIRQ(UART_WIRELESS_INST_INT_IRQN);

    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(&UART_CAMERA_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &CameraCommandBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, BufferSize);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    while (false == DL_DMA_isChannelEnabled(DMA, DMA_CH0_CHAN_ID));
}

//串口发送单个字符
void Uart0_Send_Char(char ch)
{
    while(DL_UART_isBusy(UART_INST) == true);
    DL_UART_Main_transmitData(UART_INST, ch);
}

//串口发送字符串
void Uart0_Send_String(char* str)
{
    while(str!=0 && *str!=0)
    {
        Uart0_Send_Char(*str++);
    }
}

//串口打印
void Uart0_Printf(const char* format, ...)
{
    char tmp[128];  
    va_list argptr;

    va_start(argptr, format);
    vsnprintf(tmp, sizeof(tmp) - 1, format, argptr);
    va_end(argptr);

    Uart0_Send_String(tmp);
}
//
//@简介：增加写索引
//@参数：Length:增加的长度
//
static void AddReadIndex(uint8_t Length)
{
    ReadIndex += Length;
    ReadIndex %= BufferSize;
}

static uint8_t Read(uint8_t i)
{
    uint8_t index = i % BufferSize;
    return Buffer[index];
}

//
//@简介：获取当前缓冲区数据长度
//@返回值：数据长度，最大为BufferSize - 1
//        
static uint8_t GetLength(void)
{
    return (WriteIndex + BufferSize - ReadIndex) % BufferSize;
}

//
//@简介：获取缓冲区剩余空间长度
//@返回值：剩余空间长度，最小为0
//
static uint8_t GetRemain(void)
{
    return BufferSize - GetLength() - 1;
}

//
//@简介：写入数据到缓冲区
//@参数：data:要写入的数据
//@参数：size:数据长度
//@返回值：写入成功返回1，失败返回0
//
static uint8_t Write(uint8_t* data, uint16_t size)
{
    if(size >= GetRemain()){
        return 0;
    }
    if(WriteIndex + size < BufferSize){
        memcpy(&Buffer[WriteIndex], data, size);
        WriteIndex += size;
        WriteIndex %= BufferSize;
    } else {
        uint16_t firstPart = BufferSize - WriteIndex;
        memcpy(&Buffer[WriteIndex], data, firstPart);
        uint16_t secondPart = size - firstPart;
        memcpy(Buffer, &data[firstPart], secondPart);
        WriteIndex = secondPart;
    }
    return 1;
}

//
//@简介：从缓冲区读取一个完整命令
//@参数：command:存放读取命令的缓冲区
//@返回值：命令长度，失败返回0
//
uint8_t UART_ReadCommand(uint8_t* command)
{
    while(1)
    {
        uint8_t dataLength = GetLength();
        if(dataLength < CommandMiniSize){
            return 0;
        }

        if(Buffer[ReadIndex] != CommandStartByte){
            AddReadIndex(1);
            continue;
        }

        dataLength = GetLength();
        if(dataLength < CommandMiniSize){
            return 0;
        }

        for(uint8_t i = 1; i < dataLength; i++){
            if(Buffer[(ReadIndex + i) % BufferSize] == CommandEndByte){
                dataLength = i + 1;
                break;
            }
            if(i == dataLength - 1){
                return 0;
            }
        }

        //含校验位代码
/*
        uint8_t Sum = 0;
        for(uint8_t i = 0; i < dataLength; i++){
            Sum += Read(ReadIndex + i);
        }
        if(Sum != Read(ReadIndex + dataLength - 1)){
            AddReadIndex(1);
            continue;
        }
        uint8_t cmdLength = Buffer[ReadIndex + 1];
        if(dataLength < cmdLength){
            AddReadIndex(1);
            continue;
        }
*/

        for (uint8_t i = 0; i < dataLength; i++) {
            command[i] = Read(ReadIndex + i);
        }
        AddReadIndex(dataLength);
        return dataLength;
    }
}


//uint16_t count = 0;
//uint8_t Buffer2[128];
void UART_INST_IRQHandler(void)
{
    //Uart0_Printf("Enter");
    switch( DL_UART_getPendingInterrupt(UART_INST) )
    {
        case DL_UART_IIDX_RX:
            //Uart0_Printf("Enter_");
            data = DL_UART_Main_receiveData(UART_INST);
            Write(&data, 1);
            //Buffer2[count % 128] = data;
            //count++;
            break;
        default:
            break;
    }
}

//---------------------------------------云台发送代码-------------------------------------------//
//串口发送单个字符
static void Uart_Gimbal_Send_Char(char ch)
{
    while(DL_UART_isBusy(UART_Gimbal_INST) == true);
    DL_UART_Main_transmitData(UART_Gimbal_INST, ch);
}

//串口发送字符串
void Uart_Gimbal_Send_String(char* str)
{
    while(str!=0 && *str!=0)
    {
        Uart_Gimbal_Send_Char(*str++);
    }
}

void Uart_Gimbal_Printf(const char* format, ...)
{
    char tmp[128];  
    va_list argptr;

    va_start(argptr, format);
    vsnprintf(tmp, sizeof(tmp) - 1, format, argptr);
    va_end(argptr);

    Uart_Gimbal_Send_String(tmp);
}

static void Uart_Gimbal_Send_Bytes(uint8_t* data, uint8_t length)
{
    for(uint8_t i = 0; i < length; i++){
        Uart_Gimbal_Send_Char(data[i]);
    }
}

//@简介：计算BBC校验码
//@参数：command:命令数据
//@参数：length:命令长度
//@返回值：BBC校验码
static uint8_t BBC_Get(uint8_t* command, uint8_t length)
{
    uint8_t bbc = 0;
    for(uint8_t i = 0; i < length; i++){
        bbc ^= command[i];
    }
    return bbc;
}

//@简介：通过DMA发送云台命令
//@参数：command:命令数据
//@参数：length:命令长度
//@返回值：无
void Uart_Gimbal_Send_Command_DMA(uint8_t* command, uint8_t length)
{
    //构建命令数据包
    GimbalCommandBuffer[0] = Gimbal_StartByte;
    memcpy(&GimbalCommandBuffer[1], command, length);
    GimbalCommandBuffer[length + 1] = BBC_Get(GimbalCommandBuffer, length + 1);
    GimbalCommandBuffer[length + 2] = Gimbal_EndByte;

    //配置DMA传输
    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t)(&GimbalCommandBuffer[0]));
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t)(&UART_Gimbal_INST->TXDATA));
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, length + 3);
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
}

//@简介：通过轮询发送云台命令
//@参数：command:命令数据
//@参数：length:命令长度
//@返回值：无
void Uart_Gimbal_Send_Command(uint8_t* command, uint8_t length)
{
    //构建命令数据包
    GimbalCommandBuffer[0] = Gimbal_StartByte;
    memcpy(&GimbalCommandBuffer[1], command, length);
    GimbalCommandBuffer[length + 1] = BBC_Get(GimbalCommandBuffer, length + 1);
    GimbalCommandBuffer[length + 2] = Gimbal_EndByte;
    Uart_Gimbal_Send_Bytes(GimbalCommandBuffer, length + 3);
}

void UART_Gimbal_INST_IRQHandler(void)
{
    switch( DL_UART_getPendingInterrupt(UART_Gimbal_INST) )
    {
        case DL_UART_MAIN_IIDX_DMA_DONE_TX:
            Gimbal_Transmit_Wait_DMA();
            break;
        case DL_UART_MAIN_IIDX_EOT_DONE:
            Gimbal_Transmit_Wait_EndofTransmit();
            break;
        default:
            break;
    }
}
//--------------------------------相机串口代码-------------------------------------------//
//串口发送单个字符
void Uart_Camera_Send_Char(char ch)
{
    while(DL_UART_isBusy(UART_CAMERA_INST) == true);
    DL_UART_Main_transmitData(UART_CAMERA_INST, ch);
}

//串口发送字符串
void Uart_Camera_Send_String(char* str)
{
    while(str!=0 && *str!=0)
    {
        Uart_Camera_Send_Char(*str++);
    }
}

//串口打印
void Uart_Camera_Printf(const char* format, ...)
{
    char tmp[128];
    va_list argptr;

    va_start(argptr, format);
    vsnprintf(tmp, sizeof(tmp) - 1, format, argptr);
    va_end(argptr);

    Uart_Camera_Send_String(tmp);
}


void Uart_Camera_Get_Command_Count(uint32_t* count, uint32_t* index)
{
    *count = CommandCount;
    *index = BufferSize - DL_DMA_getTransferSize(DMA, DMA_CH0_CHAN_ID);
}

static uint8_t Camera_LookupDataLen(uint8_t funcCode)
{
    for (uint8_t i = 0; i < CameraFrameDefCount; i++) {
        if (CameraFrameDefTable[i].funcCode == funcCode) {
            return CameraFrameDefTable[i].dataLen;
        }
    }
    return 0xFF;
}

static void Camera_ParseByte(uint8_t byte)
{
    switch (cameraState) 
    {
        case CAMERA_STATE_IDLE:
            if (byte == CAMERA_HEADER) {
                cameraRxBuffer[0] = byte;
                cameraRxIndex = 1;
                cameraState = CAMERA_STATE_GOT_HEADER;
            }
            break;

        case CAMERA_STATE_GOT_HEADER: {
            uint8_t dataLen = Camera_LookupDataLen(byte);
            if (dataLen == 0xFF) {
                cameraState = CAMERA_STATE_IDLE;
                if (byte == CAMERA_HEADER) {
                    cameraRxBuffer[0] = byte;
                    cameraRxIndex = 1;
                    cameraState = CAMERA_STATE_GOT_HEADER;
                }
                break;
            }
            cameraRxBuffer[1] = byte;
            cameraTotalLen = 2 + dataLen + 2;
            cameraRxIndex = 2;
            cameraState = CAMERA_STATE_RECEIVING;
            break;
        }

        case CAMERA_STATE_RECEIVING:
            cameraRxBuffer[cameraRxIndex++] = byte;
            if (cameraRxIndex >= cameraTotalLen) {
                cameraState = CAMERA_STATE_IDLE;
                uint8_t bbc = 0;
                for (uint8_t i = 0; i < cameraTotalLen - 2; i++) {
                    bbc ^= cameraRxBuffer[i];
                }
                if (bbc == cameraRxBuffer[cameraTotalLen - 2] &&
                    cameraRxBuffer[cameraTotalLen - 1] == CAMERA_TAIL) {
                    uint8_t funcCode = cameraRxBuffer[1];
                    uint8_t dataLen = cameraTotalLen - 4;
                    Camera_Receive_PutData(funcCode, &cameraRxBuffer[2], dataLen);
                }
            }
            break;
    }
}

void Uart_Camera_Send_Command(uint8_t funcCode, const uint8_t* data, uint8_t len)
{
    Uart_Camera_Send_Char(CAMERA_HEADER);
    Uart_Camera_Send_Char(funcCode);
    for (uint8_t i = 0; i < len; i++) {
        Uart_Camera_Send_Char(data[i]);
    }
    uint8_t bbc = CAMERA_HEADER ^ funcCode;
    for (uint8_t i = 0; i < len; i++) {
        bbc ^= data[i];
    }
    Uart_Camera_Send_Char(bbc);
    Uart_Camera_Send_Char(CAMERA_TAIL);
}

void Uart_Camera_ProcessDmaBuffer(void)
{
    uint32_t currentPos = BufferSize - DL_DMA_getTransferSize(DMA, DMA_CH0_CHAN_ID);
    if (currentPos == lastCameraDmaPos) {
        return;
    }

    if (currentPos > lastCameraDmaPos) {
        for (uint32_t i = lastCameraDmaPos; i < currentPos; i++) {
            Camera_ParseByte(CameraCommandBuffer[i]);
        }
    } else {
        for (uint32_t i = lastCameraDmaPos; i < BufferSize; i++) {
            Camera_ParseByte(CameraCommandBuffer[i]);
        }
        for (uint32_t i = 0; i < currentPos; i++) {
            Camera_ParseByte(CameraCommandBuffer[i]);
        }
    }

    lastCameraDmaPos = currentPos;
}

void UART_CAMERA_INST_IRQHandler(void)
{
    switch( DL_UART_getPendingInterrupt(UART_CAMERA_INST) )
    {
        case DL_UART_MAIN_IIDX_DMA_DONE_RX:
            CommandCount++;
            break;
        default:
            break;
    }
}

//--------------------------------无线串口代码-------------------------------------------//
void Uart_Wireless_Send_Char(char ch)
{
    while(DL_UART_isBusy(UART_WIRELESS_INST) == true);
    DL_UART_Main_transmitData(UART_WIRELESS_INST, ch);
}

void Uart_Wireless_Send_String(char* str)
{
    while(str!=0 && *str!=0)
    {
        Uart_Wireless_Send_Char(*str++);
    }
}

void Uart_Wireless_Printf(const char* format, ...)
{
    char tmp[128];
    va_list argptr;

    va_start(argptr, format);
    vsnprintf(tmp, sizeof(tmp) - 1, format, argptr);
    va_end(argptr);

    Uart_Wireless_Send_String(tmp);
}

void UART_WIRELESS_INST_IRQHandler(void)
{
    switch( DL_UART_getPendingInterrupt(UART_WIRELESS_INST) )
    {
        case DL_UART_IIDX_RX:
            data = DL_UART_Main_receiveData(UART_WIRELESS_INST);
            //Uart_Wireless_Send_Char(data); // 将接收到的数据发送到主串口
            break;
        default:
            break;
    }
}