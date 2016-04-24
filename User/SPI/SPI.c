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
	  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//����ʱΪ�͵�ƽ
	  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//�����ڵ�1�������ر��ɼ�
	  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//CS��Ϊ���ģʽ
	  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//2��Ƶ
	  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ֽ���ǰ
	  SPI_InitStructure.SPI_CRCPolynomial = 7;
	  SPI_Init(SPI1, &SPI_InitStructure);
	  /* Enable SPI1 */
	  SPI_Cmd(SPI1, ENABLE);
      SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);
}


void SPI1SendOneByte(uint8_t byteData)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI peripheral */
    SPI_I2S_SendData(SPI1, byteData);
}

uint8_t SPI1ReceiveOneByte(uint8_t byteData)
{
    uint8_t receiveByte=0;
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    /* Send byte through the SPI peripheral */
    receiveByte = (uint8_t)SPI_I2S_ReceiveData(SPI1);
    return receiveByte;
}

