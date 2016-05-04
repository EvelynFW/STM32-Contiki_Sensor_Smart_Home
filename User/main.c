/**

  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <string.h>
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "bsp_SysTick.h"
#include "dma.h"
#include "iwdg.h"
#include "SPI.h"
#include "delay.h"
#include "timers.h"
#include "timer4_cap.h"
#include "NVIC_CONFIG.H"
#include "cJSON.h"

#include "wifi_config.h"
#include "wifi_function.h"
#include "oled.h"
#include "dht11.h"
#include "adc.h"
#include "MQ-2.h"
#include "HC-SR501.h"
#include "HC-SR04.h"
#include "BH1750.h"
#include "RC522.h"

#include "contiki-conf.h"
#include <stdint.h>
#include <stdio.h>
#include <debug-uart.h>
#include <process.h>
#include <procinit.h>
#include <etimer.h>
#include <autostart.h>
#include <clock.h>
#include "contiki_delay.h"
#include "ProcessTask.h"


#define __CJSON_LIB_TEST__          //cJSON lib �������
//#define __CLOCK_TICK_TEST__         //NOP �� TCIK ��������
//#define __WIFI_MODULE_ON__          //WIFIģ��
//#define __OLED_MODULE_ON__          //OLED��ʾ��
//#define __DHT11_MODULE_ON__         //��ʪ�ȴ�����
//#define __MQ02_MODULE_ON__          //��������
//#define __HCSR501_MODULE_ON__       //�������͵����崫����
//#define __HCSR04_MODULE_ON__        //���������ģ��
//#define __BH1750_MODULE_ON__        //���մ�����
//#define __RC522_MODULE_ON__         //RFID������




void BSP_Config(void)
{
    /* ��ʼ�� */
    delay_init();
    clock_init();

    NVIC_Configuration_Init();
    LED_GPIO_Config();
    USART1_Config(115200);
    USART2_Config(115200);
    printf("Start Contiki OS\r\n");

#ifdef __OLED_MODULE_ON__
    OLED_Init(); //��ʼ��OLEDģ��ʹ�õĽӿں�����
    OLED_ShowString(0,0,"SPI OLED");
    OLED_ShowString(0,16,"Start OK!");
    OLED_Refresh_Gram();//������ʾ
#endif         
    
#ifdef __DHT11_MODULE_ON__
    DHT11_Init(); 
#endif   

#ifdef __MQ02_MODULE_ON__
    MQ02_Init();
#endif
    
#ifdef __HCSR501_MODULE_ON__
    HCSR501_Init();
#endif

#ifdef __HCSR04_MODULE_ON__
	UltrasonicWave_Configuration();               //�Գ�����ģ���ʼ��
#endif

#ifdef __BH1750_MODULE_ON__
	BH1750_Init();               
    BH1750_Start();
#endif

#ifdef __RC522_MODULE_ON__
	RC522_Init();
#endif

#ifdef __WIFI_MODULE_ON__
    WiFi_Config(); 
    ESP8266_STA_TCP_Client();
#endif    
    
}


int main(void)
{	
//    int* a;
//    a = (int*)malloc(sizeof(int)*10);
    BSP_Config();    
    
    IWDG_Start(2);  //wifiģ��͸��֮�������Ź�
    
    process_init();
    autostart_start(autostart_processes);
    process_start(&red_blink_process,NULL);
    process_start(&green_blink_process,NULL);
    
#ifdef __CJSON_LIB_TEST__
    process_start(&cJSON_test_process,NULL);
#endif
    
#ifdef __CLOCK_TICK_TEST__
    process_start(&clock_test_process,NULL);
#endif    

#ifdef __OLED_MODULE_ON__
    process_start(&OLED_Show_Increment_process,NULL);
#endif
    
#ifdef __DHT11_MODULE_ON__
    process_start(&DHT11_Read_Data_process,NULL);
#endif   
    
#ifdef __WIFI_MODULE_ON__     
    process_start(&wifi_send_test_process,NULL);
#endif
    
#ifdef __MQ02_MODULE_ON__     
    process_start(&MQ02_Read_Value_process,NULL);
#endif

#ifdef __HCSR501_MODULE_ON__     
    process_start(&HCSR501_Read_Status_process,NULL);
#endif

#ifdef __HCSR04_MODULE_ON__     
    process_start(&HCSR04_Measure_Distance_process,NULL);
#endif

#ifdef __BH1750_MODULE_ON__     
    process_start(&BH1750_Measure_Lumen_process,NULL);
#endif

#ifdef __RC522_MODULE_ON__     
    process_start(&RC522_Read_Card_process,NULL);
#endif

    while (1)
    {
        do
        {
        }while (process_run()>0);
    }
}





