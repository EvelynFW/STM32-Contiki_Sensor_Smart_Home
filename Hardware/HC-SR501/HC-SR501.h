#ifndef __SR501_H
#define __SR501_H	 
#include "stdbool.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
#define HCSR501_GPIO_Pin  GPIO_Pin_5
#define HCSR501_GPIO_Port GPIOB

#define HCSR501_GPIO_Read() PBin(5)// PB5


void HCSR501_Init(void);//��ʼ��
bool HCSR501_Read_Data(void);

		 				    
#endif
