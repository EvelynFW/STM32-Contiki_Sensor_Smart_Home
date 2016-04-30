#include <stdarg.h>
#include "SPI.h"
#include "wifi_config.h"



/**
*@brief SPI Initialization
**/
void SPI1_Initialization(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* NSS---->GPIO(LED) */
    SPI_SSOutputCmd(SPI1, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* SPI1 Config -------------------------------------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//ȫ˫��ͨ��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//������
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8λ����֡
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//����ʱΪ�͵�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//�����ڵ�1�������ر��ɼ�
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;//CS��Ϊ���ģʽ
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;//64��Ƶ
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ֽ���ǰ
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,DISABLE);
    SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_ERR,DISABLE);
    SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,DISABLE);


    /* Enable SPI1 */
    SPI_Cmd(SPI1, ENABLE);
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_TXE);
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);

}


/**
*@brief SPI Initialization
**/
void SPI2_Initialization(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    
    /* NSS---->GPIO(LED) */
    SPI_SSOutputCmd(SPI2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    /* SPI2 Config -------------------------------------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//���ӻ�ֻ����
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;//������
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8λ����֡
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//����ʱΪ�͵�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//�����ڵ�1�������ر��ɼ�
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;//CS��Ϊ���ģʽ
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;//64��Ƶ
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ֽ���ǰ
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,DISABLE);
    SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE,ENABLE);


    /* Enable SPI2 */
    SPI_Cmd(SPI2, ENABLE);
    SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_TXE);
    SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_RXNE);
}


void SPI1SendOneByte(uint8_t byteData)
{
    int count = 0;
    int COUNT_MAX = 50;
    
     /* �жϷ��ͻ����Ƿ�Ϊ�� */ 
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return;
        }
    }
    /* �����ֽ� */
    SPI_I2S_SendData(SPI1, byteData);  
    /* �жϽ��ջ����Ƿ�Ϊ��*/
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return;
        }
    }
    /* flush data read during the write */ 
    SPI_I2S_ReceiveData(SPI1); 
}

uint8_t SPI1ReceiveOneByte(void)
{
    uint8_t receiveByte=0;
  
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);/* Loop while DR register in not emplty */
    receiveByte = (uint8_t)SPI_I2S_ReceiveData(SPI1);
    return receiveByte;
}


void SPI2SendOneByte(uint8_t byteData)
{
    SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
    SPI_I2S_SendData(SPI2, byteData);    /* Send byte through the SPI peripheral */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);    /* Loop while DR register in not emplty */
}

uint8_t SPI2ReceiveOneByte(void)
{
    uint8_t receiveByte=0;
  
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);/* Loop while DR register in not emplty */
    receiveByte = (uint8_t)SPI_I2S_ReceiveData(SPI2);
    return receiveByte;
}

/**
* @brief This function handles SPI1 global interrupt.
*/
void SPI1_IRQHandler(void)
{
    uint8_t SPI_Value=0;
    if( SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE) == SET )
    {
        SPI_I2S_SendData(SPI1,0xFF);      
    }
    if( SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET )   
    {     
       SPI_Value = SPI_I2S_ReceiveData(SPI1);    
    }
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_TXE);
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);
}





void SPI2_IRQHandler(void)
{   
    uint8_t SPI_Value;

   int count = 0;
    int COUNT_MAX = 50;
    
     /* �жϷ��ͻ����Ƿ�Ϊ�� */ 
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return;
        }
    }
    /* �����ֽ� */
    SPI_I2S_SendData(SPI2, 0xFF);  
    /* �жϽ��ջ����Ƿ�Ϊ��*/
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return;
        }
    }
    /* flush data read during the write */ 
    SPI_Value = SPI_I2S_ReceiveData(SPI2); 
    
    switch(SPI_Value)
    {
        case 0x01:
            printf("����STM32F103VCT6���������ӳɹ�!\r\n");
        break;
        
        case 0x02:
            printf("����STM32F302-NUCLEO������!\r\n");
        break;
        
        case 0x03:
            printf("������Ϣ����ʱ���Ըɻ�!\r\n");
        break;
        
        case 0x04:
            printf("�յ�ָ��!��ʼ������...\r\n");
        break;
    }
}



