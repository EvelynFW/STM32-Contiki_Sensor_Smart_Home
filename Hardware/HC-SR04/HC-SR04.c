/******************** (C) 1209 Lab **************************
 * �ļ���  : HC-SR04.c
 * ����    �����������ģ�飬UltrasonicWave_Configuration��������
             ��ʼ������ģ�飬UltrasonicWave_StartMeasure��������
			 ������࣬������õ�����ͨ������1��ӡ����         
 * ʵ��ƽ̨��Mini STM32������  STM32F103RBT6
 * Ӳ�����ӣ�------------------
 *          | PC8  - TRIG      |
 *          | PC7  - ECHO      |
 *           ------------------
 * ��汾  ��ST3.5.0
 *
 * ����    ��Lee 
*********************************************************************************/

#include "HC-SR04.h"
#include "timers.h"
#include "delay.h"


  

float UltrasonicWave_Distance;      //������ľ���    


/*
 * ��������UltrasonicWave_Configuration
 * ����  ��������ģ��ĳ�ʼ��
 * ����  ����
 * ���  ����	
 */
void UltrasonicWave_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	//�ر�jtag
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = HCSR04_TRIG_PIN;					 //PC8��TRIG
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     //��Ϊ�������ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
    GPIO_Init(HCSR04_TRIG_PORT, &GPIO_InitStructure);	                 //��ʼ������GPIO 

    GPIO_InitStructure.GPIO_Pin = HCSR04_ECHO_PIN;				     //PC7��ECH0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		 //��Ϊ����
    GPIO_Init(HCSR04_ECHO_PORT,&GPIO_InitStructure);						 //��ʼ��GPIOA

    //  �ж����Լ��жϳ�ʼ������
    GPIO_EXTILineConfig(HCSR04_EXTI_PortSource,HCSR04_EXTI_PinSource);
    EXTI_InitStructure.EXTI_Line=HCSR04_EXTI_Line;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�½����ж�
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = HCSR04_EXTI_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�2 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�2 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
   
}


void HCSR04_EXTI_IRQHandler(void)
{
    if(EXTI_GetITStatus(HCSR04_EXTI_Line) != RESET)
    {
        EXTI_ClearITPendingBit(HCSR04_EXTI_Line);  //���EXTI7��·����λ
        TIM_SetCounter(TIM2,0);
        TIM_Cmd(TIM2, ENABLE);                                             //����ʱ��
        while(GPIO_ReadInputDataBit(HCSR04_ECHO_PORT,HCSR04_ECHO_PIN));	                 //�ȴ��͵�ƽ
        TIM_Cmd(TIM2, DISABLE);			                                 //��ʱ��2ʹ��
        UltrasonicWave_Distance=TIM_GetCounter(TIM2)*5*34/100.00;		//������� ��λcm
        if(UltrasonicWave_Distance>0 && UltrasonicWave_Distance<300)
        {
            printf("Distance: %f cm\r\n",UltrasonicWave_Distance);
        }
    }
}

/*
 * ��������UltrasonicWave_StartMeasure
 * ����  ����ʼ��࣬����һ��>10us�����壬Ȼ��������صĸߵ�ƽʱ��
 * ����  ����
 * ���  ����	
 */
void UltrasonicWave_StartMeasure(void)
{
  GPIO_SetBits(HCSR04_TRIG_PORT,HCSR04_TRIG_PIN); 		  //��>10US�ĸߵ�ƽ
  Delay_NOP_us(20);		                      //��ʱ20US
  GPIO_ResetBits(HCSR04_TRIG_PORT,HCSR04_TRIG_PIN); 
}

/******************* (C) 1209 Lab *****END OF FILE************/
