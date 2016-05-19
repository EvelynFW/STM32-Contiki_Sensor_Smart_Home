#include "Uint8PacketQueueManger.h"


/*����һ�����Ͱ��Ķ���
*����һ���������ָ��
*/
Uint8PacketQueue* CreatUint8PacketQueue(Uint8PacketNode* (*CreatUint8PacketNode)(uint8_t* packet, void* packetBlock))
{
    Uint8PacketQueue*  Uint8PacketQueueHandle = NULL;
    Uint8PacketQueueHandle = (Uint8PacketQueue*)malloc(sizeof(Uint8PacketQueue));//��Ϊȫ�ֵĹ�����ж���ֻ����һ�Σ����ͷ�
    if(Uint8PacketQueueHandle == NULL)
    {
        return NULL;
    }
    Uint8PacketQueueHandle->head = NULL;
    Uint8PacketQueueHandle->last = NULL;
    Uint8PacketQueueHandle->CreatUint8PacketNode = CreatUint8PacketNode;
    return Uint8PacketQueueHandle;
}



/*��һ�����Ͱ��ڵ�ѹ���������
*Uint8PacketQueueHandle:Ҫѹ��Ĺ����ָ��
*uint8PacketNodePointer:���Ͱ��ڵ�
*/
void Uint8PacketQueuePush(Uint8PacketQueue* Uint8PacketQueueHandle,Uint8PacketNode* uint8PacketNodePointer)
{
    if(Uint8PacketQueueHandle->head == NULL)
    {
        Uint8PacketQueueHandle->head = uint8PacketNodePointer;
    }
    else
    {
        Uint8PacketQueueHandle->last->next = uint8PacketNodePointer;
    }
    uint8PacketNodePointer->next = NULL;
    Uint8PacketQueueHandle->last = uint8PacketNodePointer;
}
/*ͨ�������ݻ������ݿ鴴��һ�����ڵ㣬���ڼ�����еȲ���
*packet:���ݰ�
*packetBlock:���ݿ�
*�����½��İ��ڵ�ָ��
*
*/
Uint8PacketNode* CreatUint8PacketNode(uint8_t* packet, void* packetBlock)
{
    Uint8PacketNode* uint8PacketNodePointer;
    
    uint8PacketNodePointer = (Uint8PacketNode*)malloc(sizeof(Uint8PacketNode));
    if(!uint8PacketNodePointer)return NULL;
    
    uint8PacketNodePointer->packet = packet;
    uint8PacketNodePointer->packetLength = 0;
    uint8PacketNodePointer->packetBlock = packetBlock;
    uint8PacketNodePointer->next = NULL;
    uint8PacketNodePointer->index = 0;
    uint8PacketNodePointer->resendCount = 0;
    uint8PacketNodePointer->resendTime = 0;
    return uint8PacketNodePointer;
}

Uint8PacketNode* SetUint8PacketNodeLength(Uint8PacketNode* uint8PacketNodePointer,uint16_t packetLength)
{
    if(!uint8PacketNodePointer)return NULL;
    uint8PacketNodePointer->packetLength = packetLength;
    return uint8PacketNodePointer;
}

/*��һ�����ݰ��ṹ��ڵ�ѹ���������
*Uint8PacketQueueHandle:Ҫѹ��Ĺ����ָ��
*packetBlock:���ݰ�ָ��
*
*
*/
void Uint8PacketQueuePushBlock(Uint8PacketQueue* Uint8PacketQueueHandle,void* packetBlock)
{
    Uint8PacketNode* uint8PacketNodePointer;
    if(!Uint8PacketQueueHandle)return;
    uint8PacketNodePointer = Uint8PacketQueueHandle->CreatUint8PacketNode(NULL, packetBlock);
    Uint8PacketQueuePush(Uint8PacketQueueHandle, uint8PacketNodePointer);
}

/*��һ�����ݰ��ڵ�ѹ���������
*Uint8PacketQueueHandle:Ҫѹ��Ĺ����ָ��
*packet:���ݰ�ָ��
*
*
*/
void Uint8PacketQueuePushStreamData(Uint8PacketQueue* Uint8PacketQueueHandle,uint8_t* packet,uint16_t packetLength)
{
    Uint8PacketNode* uint8PacketNodePointer;
    if(!Uint8PacketQueueHandle)return;
    uint8PacketNodePointer = Uint8PacketQueueHandle->CreatUint8PacketNode(packet, NULL);
    SetUint8PacketNodeLength(uint8PacketNodePointer, packetLength);
    Uint8PacketQueuePush(Uint8PacketQueueHandle, uint8PacketNodePointer);
}

/*�ӷ��Ͱ������е���һ���ڵ�
*Uint8PacketQueueHandle:Ҫ�����Ĺ����ָ�롣
*����ֵΪ�ڵ�ָ�롣
*
*
*/
Uint8PacketNode* Uint8PacketQueuePop(Uint8PacketQueue* Uint8PacketQueueHandle)
{
    Uint8PacketNode* uint8PacketNodePointer;
    if(Uint8PacketQueueHandle->head == NULL)
    {
        return NULL;
    }
    uint8PacketNodePointer = Uint8PacketQueueHandle->head;
    Uint8PacketQueueHandle->head = Uint8PacketQueueHandle->head->next;
    uint8PacketNodePointer->next = NULL;
    return uint8PacketNodePointer;  
}










