#ifndef __HCSR04_H
#define	__HCSR04_H
#include "stm32f10x.h"
#include <stdio.h>
#include <stdbool.h>


#define	HCSR04_TRIG_PORT        GPIOA		    //TRIG       
#define	HCSR04_TRIG_PIN         GPIO_Pin_11     //TRIG  

#define	HCSR04_ECHO_PORT        GPIOA		    //ECHO 
#define	HCSR04_ECHO_PIN         GPIO_Pin_12	    //ECHO 

#define HCSR04_EXTI_PortSource  GPIO_PortSourceGPIOA
#define HCSR04_EXTI_PinSource   GPIO_PinSource12
#define HCSR04_EXTI_Line        EXTI_Line12
#define HCSR04_EXTI_IRQn        EXTI15_10_IRQn
#define HCSR04_EXTI_IRQHandler  EXTI15_10_IRQHandler


void UltrasonicWave_Configuration(void);               //�Գ�����ģ���ʼ��
void UltrasonicWave_StartMeasure(void);                //��ʼ��࣬����һ��>10us�����壬Ȼ��������صĸߵ�ƽʱ��

#endif /* __UltrasonicWave_H */

