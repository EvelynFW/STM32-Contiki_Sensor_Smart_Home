#include "dma.h"
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


   
//DMA1�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//Channel_x:DMAͨ��CHx
//PeripheralBaseAddr:�����ַ
//MemoryBaseAddr:�洢����ַ
//BufferSize:���ݴ����� 
void MYDMA_Config(DMA_Channel_TypeDef* Channel_x,u32 PeripheralBaseAddr,u32 MemoryBaseAddr,FunctionalState isMemoryToPeripheral,u16 BufferSize)
{
    DMA_InitTypeDef DMA_InitStructure;
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	                //ʹ��DMA����
    DMA_DeInit(Channel_x);                                          //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = PeripheralBaseAddr;  //DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = MemoryBaseAddr;  		//DMA�ڴ����ַ
    DMA_InitStructure.DMA_BufferSize = BufferSize;  			    //DMAͨ����DMA����Ĵ�С
    
    if(isMemoryToPeripheral == ENABLE)
    {
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //�����ַ�Ĵ�������
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             //�ڴ��ַ�Ĵ�������
    }
    else
    {
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  //���ݴ��䷽�򣬴������ȡ���͵��ڴ�
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;    //�����ַ�Ĵ�������
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;             //�ڴ��ַ�Ĵ�������
    }
    
    
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //�������ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;     //�ڴ����ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                       //��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;               //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                        //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(Channel_x, &DMA_InitStructure);                        //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ������ʶ�ļĴ���
	//DMA_Cmd(DMA_CHx, ENABLE);                                         //ʹ����ָʾ��ͨ�� ��ÿ�η�������ʱʹ��
}
//����һ��DMA����
void MYDMA_Enable(DMA_Channel_TypeDef* DMA_CHx, u16 bufferSize)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //�ر�USART1 TX DMA1 ��ָʾ��ͨ��      
 	DMA_SetCurrDataCounter(DMA_CHx,bufferSize);//DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA_CHx, ENABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}	  


void USART1_TXD_DMA_Enable(u16 bufferSize)
{
 	MYDMA_Enable(USART1_DMA_Channel,bufferSize);//DMAͨ����DMA����Ĵ�С
}

 
void USART2_TXD_DMA_Enable(u16 bufferSize)
{
 	MYDMA_Enable(USART2_DMA_Channel,bufferSize);//DMAͨ����DMA����Ĵ�С
}





















