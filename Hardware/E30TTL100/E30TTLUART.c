#include "E30TTLUART.h"
#include "delay.h"


uint16_t E30TTLUART_Appoint_Target_Address = 0x0000;
uint8_t E30TTLUART_Appoint_Target_Channel = 0x50;

void (*E30TTLUART_SendBytesData)(uint8_t *USART3_SendBuff, uint16_t DataSendLength);
uint8_t E30TTLUART_AppointSendMaxLength = 58;//ָ������ʱ�ְ���С    

void E30TTLUART_Init(void)
{	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
 	GPIO_InitStructure.GPIO_Pin = E30TTLUART_M0_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(E30TTLUART_M0_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = E30TTLUART_M1_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(E30TTLUART_M1_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = E30TTLUART_AUX_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //��������
 	GPIO_Init(E30TTLUART_AUX_Port, &GPIO_InitStructure);

    GPIO_ResetBits(E30TTLUART_M0_Port,E30TTLUART_M0_Pin);
    GPIO_ResetBits(E30TTLUART_M1_Port,E30TTLUART_M1_Pin);

} 

void E30TTLUART_ChangeMode(uint8_t E30TTLUART_mode)
{
    while(E30TTLUART_AUX_IN == 0);//Ϊ�͵�ƽ��˵��æ����˵ȴ�
    if(E30TTLUART_mode>3)return;
    E30TTLUART_M0_OUT = E30TTLUART_mode & 0x01;
    E30TTLUART_M1_OUT = (E30TTLUART_mode>>1) & 0x01;
    Delay_NOP_ms(2);
    while(E30TTLUART_AUX_IN == 0);
}

void E30TTLUART_Resetting()//ģ��ָ�λ
{
    E30TTLUART_ChangeMode(3);
    E30TTLUART_SendOneByte(0xC4);//��λָ��
    E30TTLUART_SendOneByte(0xC4);
    E30TTLUART_SendOneByte(0xC4);
    Delay_NOP_ms(2);
    while(E30TTLUART_AUX_IN == 0);
}

void E30TTLUART_Config(uint16_t E30TTLUART_Address,uint8_t E30TTLUART_Channel, FunctionalState isAppointTargetSend)    //����ʱ������9600��ģ�鹤������������Ϊ115200
{
    uint8_t count;
    uint8_t configParameter[6] = {0xC0,0x00,0x00,0x3F,0x50,0x44};//Ĭ�ϳ�ʼ���ò���ֵ
    E30TTLUART_ChangeBaudRate(9600,DISABLE);
    E30TTLUART_ChangeMode(E30TTLUART_MODE_3);
    
    configParameter[2] = (uint8_t)(E30TTLUART_Address & 0x00FF);
    configParameter[1] = (uint8_t)(E30TTLUART_Address>>8);
    configParameter[4] = E30TTLUART_Channel;

    if(isAppointTargetSend == ENABLE)
    {
        configParameter[5] |= 0x80;//���λ��1Ϊ���㴫��
        E30TTLUART_SendBytesData = E30TTLUART_AppointSendBytesData;
    }
    else
    {
        configParameter[5] &= 0x7F;
        E30TTLUART_SendBytesData = E30TTLUART_TransparentSendBytesData;
    }
    
    for(count=0;count<sizeof(configParameter);count++)
    {
        E30TTLUART_SendOneByte(configParameter[count]);
    }
    
    E30TTLUART_Resetting();//�ı������Ҫ��λ
    
    E30TTLUART_ChangeMode(E30TTLUART_MODE_0);
    E30TTLUART_ChangeBaudRate(115200,ENABLE);    
}
void E30TTLUART_TransparentSendBytesData(uint8_t *bytesBuf, uint16_t bytesBufLength)//���ͺ������Թ����ĺ����ض�
{
    while(bytesBufLength > 0xFF)
    {
        while(E30TTLUART_AUX_IN == 0);
        E30TTLUART_UART_SendBytesBuf(bytesBuf, 0xFF);
        bytesBuf += 0xFF;
        bytesBufLength -= 0xFF;
        Delay_NOP_us(10);
    }
    while(E30TTLUART_AUX_IN == 0);
    E30TTLUART_UART_SendBytesBuf(bytesBuf, bytesBufLength);
}

void E30TTLUART_Point2PointSendBytesData(uint16_t address,uint8_t channel,uint8_t* bytesBuf, uint16_t bytesBufLength)//��Ե㷢�ͣ���Ҫ��ַ���ŵ�����
{
    uint8_t targetPosition[3];
    uint8_t count;
    
    targetPosition[1] = (uint8_t)(address & 0x00FF);
    targetPosition[0] = (uint8_t)(address>>8);
    targetPosition[2] = channel;
    
  
    while(bytesBufLength > E30TTLUART_AppointSendMaxLength)
    {
        while(E30TTLUART_AUX_IN == 0);
        for(count=0;count<sizeof(targetPosition);count++)E30TTLUART_SendOneByte(targetPosition[count]);
        E30TTLUART_UART_AppointSendBytesBuf(bytesBuf, E30TTLUART_AppointSendMaxLength);

        bytesBuf += E30TTLUART_AppointSendMaxLength;
        bytesBufLength -= E30TTLUART_AppointSendMaxLength;
        Delay_NOP_us(10);
    }
    while(E30TTLUART_AUX_IN == 0);
    for(count=0;count<sizeof(targetPosition);count++) E30TTLUART_SendOneByte(targetPosition[count]);
    E30TTLUART_UART_AppointSendBytesBuf(bytesBuf, bytesBufLength); 
}

void E30TTLUART_AppointSendBytesData(uint8_t *bytesBuf, uint16_t bytesBufLength)
{
    E30TTLUART_Point2PointSendBytesData(E30TTLUART_Appoint_Target_Address, E30TTLUART_Appoint_Target_Channel, bytesBuf, bytesBufLength);
}
    





