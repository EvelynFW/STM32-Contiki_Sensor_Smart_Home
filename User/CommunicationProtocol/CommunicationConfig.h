#ifndef __COMMUNICATION_CONFIG_H__
#define __COMMUNICATION_CONFIG_H__


#include "CommunicationProtocol.h"
#include "ProtocolQueueManger.h"

//�����ǽڵ�ģʽ�����ն�ģʽ��__TERMINAL_ON__��ʾ�ն�ģʽ��__TERMINAL_OFF__��ʾ�ڵ�ģʽ
#define __TERMINAL_ON__



//ʹ�����÷��Ͷ˿�  
//#define sendUartByteBuf   USART1_DMA_Send_Data
#ifdef __TERMINAL_ON__
//    #define sendUartByteBuf   USART2_DMA_Send_Data
    #define sendUartByteBuf   SendUSART2BytesBuf
#else
    #ifdef __TERMINAL_OFF__
    #define sendUartByteBuf   E30TTLUART_SendBytesData
    #endif
#endif



//�����ط������볬ʱʱ��
#define PROTOCOL_PACKET_RESENT_COUNT_MAX    2
#define PROTOCOL_PACKET_RESENT_TIME_MAX     1

extern const uint16_t Protocol_LoopbackAddress;
extern const uint16_t Protocol_LocalhostAddress;
extern const uint16_t Protocol_BroadcastAddress;
extern const uint16_t Protocol_GatewayAddress;




#endif






