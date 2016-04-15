/**

  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "wifi_config.h"
#include "wifi_function.h"
#include "bsp_SysTick.h"
#include "dma.h"
#include <string.h>
 
 
void ( * pNet_Test ) ( void );


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
    /* ��ʼ�� */
    WiFi_Config();                                                                  //��ʼ��WiFiģ��ʹ�õĽӿں�����
    SysTick_Init();                   /* ��ʱ������ʱ�ӳ�ʼ�� */
    
    ESP8266_STA_TCP_Client();

    while(1)
    {
        Delay_ms(500);
        GPIO_SetBits(GPIOA, GPIO_Pin_8);
        UART2_SendBuff = " asdfafasdf";
        UART2_DMA_Send_Data(UART2_SendBuff, 11);
        Delay_ms(500);
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
        UART2_SendBuff = " 21398416";
        UART2_DMA_Send_Data(UART2_SendBuff, 11);
    }
}


/*********************************************END OF FILE**********************/
