#ifndef __COMMUNICATION_PROTOCOL_H__
#define __COMMUNICATION_PROTOCOL_H__

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "sys.h"   //��STM32��ص�һЩ����
#include "stdlib.h"
#include <string.h>
#include <stdbool.h>
#include "cJSON.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "FIFO.h"
#include "E30TTLUART.h"


#define PROTOCOL_PACKET_CONSISTENT_LENGTH   15
#define PROTOCOL_PACKET_HEAD_LENGTH         4
#define PROTOCOL_PACKET_RESENT_COUNT_MAX    2
#define PROTOCOL_PACKET_RESENT_TIME_MAX     1

//ʹ�ô���1����2����  
//#define sendUartByteBuf   USART1_DMA_Send_Data
#define sendUartByteBuf   USART2_DMA_Send_Data
//#define sendUartByteBuf   E30TTLUART_SendBytesData



typedef enum FunctionWord_TypeDef
{ 
    FunctionWord_Null           = 0x00,
    FunctionWord_Handshake      = 0xF0,
    FunctionWord_Acknowledgement, 
    FunctionWord_RegisterDevice,
    FunctionWord_Dormant,
    FunctionWord_StartUP,
    FunctionWord_Data,
    FunctionWord_Logout,
    FunctionWord_Reboot,
    FunctionWord_Shutdown
}FunctionWord_TypeDef;

typedef struct PacketBlock
{
    uint8_t head[PROTOCOL_PACKET_HEAD_LENGTH];
    uint16_t targetAddress;
    uint16_t sourceAddress;
    uint16_t index;
    FunctionWord_TypeDef functionWord;
    uint8_t headCheckSum;
    uint16_t messageDataLength;
    uint8_t* messageData;
    uint8_t messageDataCheckSum;
}PacketBlock;


typedef struct Uint8PacketNode
{
    uint8_t* packet;
    PacketBlock* packetBlock;
    struct Uint8PacketNode* next;
    uint16_t index;
    uint8_t resendCount;
    uint8_t resendTime;
}Uint8PacketNode;

typedef struct Uint8PacketQueue
{
    Uint8PacketNode* head;
    Uint8PacketNode* last;   
}Uint8PacketQueue;



Uint8PacketQueue* CreatUint8PacketQueue(void);

void SendUnsentPacketQueue(void);
void SendUnackedPacketQueue(void);
void IncreaseUnackedPacketQueueResendTime(void);

void AssembleProtocolPacketPushSendQueue(uint16_t targetAddress, FunctionWord_TypeDef functionWord, uint16_t messageDataLength,uint8_t* messageData);
void PushReceiveByteDataIntoReceiveFIFO(uint8_t streamByteData);
void LoadReceiveQueueByteToPacketBlock(void);
void DealWithReceivePacketQueue(void);

#endif















