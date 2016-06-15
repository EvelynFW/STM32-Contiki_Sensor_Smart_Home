#include "HC-SR501.h"


//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void HCSR501_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB,PE�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = HCSR501_GPIO_Pin;				 //LED0-->PB.5 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ; 		 //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(HCSR501_GPIO_Port, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
}
 

bool HCSR501_Read_Data()
{
    return HCSR501_GPIO_Read();
}



