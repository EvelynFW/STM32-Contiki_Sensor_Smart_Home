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
    SysTick_Init();                                                                 //���� SysTick Ϊ 1ms �ж�һ�� 

    PC_Usart("\r\nESP8266 WiFiģ�����\r\n");                            //��ӡ����������ʾ��Ϣ


    ESP8266_STA_TCP_Client();

    while(1);
	
}


/*********************************************END OF FILE**********************/
