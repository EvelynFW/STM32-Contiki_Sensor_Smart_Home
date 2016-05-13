#ifndef __DMA_H
#define	__DMA_H	   
#include "stm32f10x.h"
#include "stm32f10x_dma.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK miniSTM32������
//DMA ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/8
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
							    					    
#define USART1_TX_DMA_Channel             DMA1_Channel4                                                   
#define USART2_TX_DMA_Channel             DMA1_Channel7
#define USART3_TX_DMA_Channel             DMA1_Channel2

void MYDMA_Config(DMA_Channel_TypeDef* Channel_x,u32 PeripheralBaseAddr,u32 MemoryBaseAddr,FunctionalState isMemoryToPeripheral,u16 BufferSize);
void MYDMA_Enable(DMA_Channel_TypeDef* DMA_CHx, u16 bufferSize);
void USART1_TXD_DMA_Enable(u16 bufferSize);
void USART2_TXD_DMA_Enable(u16 bufferSize);
void USART3_TXD_DMA_Enable(u16 bufferSize);


#endif




