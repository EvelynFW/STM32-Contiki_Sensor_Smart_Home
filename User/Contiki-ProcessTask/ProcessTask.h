#ifndef __PROCESS_TASK_H__
#define __PROCESS_TASK_H__
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "bsp_usart3.h"
#include "bsp_uart4.h"
#include "bsp_uart5.h"
#include "bsp_SysTick.h"
#include "dma.h"
#include "iwdg.h"
#include "SPI.h"
#include "delay.h"
#include "timers.h"
#include "timer4_cap.h"
#include "NVIC_CONFIG.H"
#include "cJSON.h"
#include "stm_flash.h"
#include "Filters.h"
#include "CommunicationProtocol.h"
#include "CommunicationConfig.h"

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
#include "E30TTLUART.h"
#include "SDS01.h"
#include "SHT15.h"
#include "T6603.h"
#include "W5500.h"
#include "HX711.h"
#include "Keyboard_4x5.h"
#include "Steelyard.h"

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


//��"CommunicationConfig.h"�ļ����޸� __TERMINAL_XX__�궨�壬ѡ��ڵ���ն�ģʽ
#ifdef __TERMINAL_ON__
//    #define __SDS01_MODULE_ON__         //SDS01 PM2.5 PM10 ������ģ��
//    #define __SHT15_MODULE_ON__         //SHT15 ��ȷ��ʪ�ȴ�����ģ��
//    #define __WIFI_MODULE_ON__          //WIFIģ�鿪��
    #define __HX711_MODULE_ON__         //HX711ģ�鿪��
    #define __OLED_MODULE_ON__          //OLED��ʾ��
    #define __KEYBOARD_MODULE_ON__      //�������ģ��
    #define __LED_BLINK_ON__            //LED ��˸
//    #define __E30TTLUART_MODULE_ON__    //E30���ߴ���ģ��
//    #define __W5500_MODULE_ON__         //W5500ģ�鿪��
//    #define __COMMUNICAT_PROTOCOL__     //�����Ͷ���
//    #define __COMMUNICAT_PROTOCOL_SENSOR_DATA__  //ͨ��JSON������������
#else
    #ifdef __TERMINAL_OFF__
    #define __COMMUNICAT_PROTOCOL__     //�����Ͷ���
    #define __COMMUNICAT_PROTOCOL_SENSOR_DATA__  //ͨ��JSON������������
    #define __OLED_MODULE_ON__          //OLED��ʾ��
    #define __DHT11_MODULE_ON__         //��ʪ�ȴ�����
    #define __MQ02_MODULE_ON__          //��������
    #define __HCSR501_MODULE_ON__       //�������͵����崫����
    #define __HCSR04_MODULE_ON__        //���������ģ��
    #define __BH1750_MODULE_ON__        //���մ�����
    #define __RC522_MODULE_ON__         //RFID������
    #define __SDS01_MODULE_ON__         //SDS01 PM2.5 PM10 ������ģ��
//    #define __SHT15_MODULE_ON__         //SHT15 ��ȷ��ʪ�ȴ�����ģ��
    #define __T6603_MODULE_ON__         //T6603-5 CO2������ģ��
    #define __E30TTLUART_MODULE_ON__    //E30���ߴ���ģ��
    #endif
#endif

//    #define __CLOCK_TICK_TEST__         //NOP��ʱʱ�����
//    #define __W5500_SEND_TEST_ON__      //W5500ģ�鷢�Ͳ��Կ���

PROCESS_NAME(red_blink_process);
PROCESS_NAME(green_blink_process);
PROCESS_NAME(IWDG_Feed_process);
PROCESS_NAME(Communication_Protocol_Send_process);
PROCESS_NAME(clock_test_process);
PROCESS_NAME(cJSON_test_process);
PROCESS_NAME(CommunicatProtocol_Send_Sensor_Data);

PROCESS_NAME(wifi_send_test_process);
PROCESS_NAME(OLED_Show_Increment_process);
PROCESS_NAME(DHT11_Read_Data_process);
PROCESS_NAME(MQ02_Read_Value_process);
PROCESS_NAME(HCSR501_Read_Status_process);
PROCESS_NAME(HCSR04_Measure_Distance_process);
PROCESS_NAME(BH1750_Measure_Lumen_process);
PROCESS_NAME(RC522_Read_Card_process);
PROCESS_NAME(SDS01_Read_PM_Value_process);
PROCESS_NAME(SHT15_Read_DATA_Value_process);
PROCESS_NAME(T6603_Read_CO2_PPM_process);
PROCESS_NAME(W5500_send_test_process);
PROCESS_NAME(HX711_read_weight_process);
PROCESS_NAME(KEYBOARD_Scan_process);
PROCESS_NAME(Steelyard_Display_Peeling_Error_process);

extern process_event_t keyboard_press_event;
#endif

