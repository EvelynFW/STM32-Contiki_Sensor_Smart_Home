#include "CommunicationProtocol.h"


#define AT_LEAST_PACKET_BYTES_LENGTH 8

const uint8_t Protocol_PacketHeadData[2] = {0xAA,0xAA};
uint16_t Protocol_PacketSendIndex = 0;//�������ӵķ������
uint16_t Protocol_PacketAckedIndex = 0;//��Ϊ������ת�Ľ������
Uint8PacketQueue UnsentPacketQueue;
Uint8PacketQueue UnackedPacketQueue;
Uint8PacketQueue ReceivedPacketQueue;
Uint8FIFOQueue ReceiveBytesFIFOQueue;

Uint8PacketQueue* UnsentPacketQueueHandle = &UnsentPacketQueue;
Uint8PacketQueue* UnackedPacketQueueHandle = &UnackedPacketQueue;
Uint8PacketQueue* ReceivedPacketBlockQueueHandle = &ReceivedPacketQueue;
Uint8FIFOQueue* ReceiveBytesFIFOQueueHandle = &ReceiveBytesFIFOQueue;

//uint8_t Protocol_DeviceAddressData[2] = {0x00,0x00};
//FunctionWord_TypeDef Protocol_FunctionWord;
//uint8_t Protocol_DataLengthBytes[2];
//uint8_t* Protocol_JSONMessageData;//�˴�uint8_t ����Ϊ��MDK�У�Ĭ��char ��unsigned char
//uint8_t Protocol_PacketCheckSum;



/*����һ�����Ͱ��Ķ���
*����һ���������ָ��
*
*
*
*/
Uint8PacketQueue* CreatUint8PacketQueue(void)
{
    Uint8PacketQueue*  Uint8PacketQueueHandle = NULL;
    Uint8PacketQueueHandle = (Uint8PacketQueue*)malloc(sizeof(Uint8PacketQueue));//��Ϊȫ�ֵĹ�����ж���ֻ����һ�Σ����ͷ�
    if(Uint8PacketQueueHandle == NULL)
    {
        return NULL;
    }
    Uint8PacketQueueHandle->head = NULL;
    Uint8PacketQueueHandle->last = NULL;
    return Uint8PacketQueueHandle;
}

/*��һ�����Ͱ��ڵ�ѹ���������
*Uint8PacketQueueHandle:Ҫѹ��Ĺ����ָ��
*uint8PacketNodePointer:���Ͱ��ڵ�
*
*
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

Uint8PacketNode* CreatUint8PacketNode(uint8_t* packet, PacketBlock* packetBlock)
{
    Uint8PacketNode* uint8PacketNodePointer;
    uint8_t packetIndexPosition;
    packetIndexPosition = sizeof(((PacketBlock*)0)->PacketHead);
    
    uint8PacketNodePointer = (Uint8PacketNode*)malloc(sizeof(Uint8PacketNode));
    if(!uint8PacketNodePointer)return NULL;
    
    uint8PacketNodePointer->packet = packet;
    uint8PacketNodePointer->packetBlock = packetBlock;
    uint8PacketNodePointer->next = NULL;
    if(packet)
    {
        uint8PacketNodePointer->index = packet[packetIndexPosition];
        uint8PacketNodePointer->index = packet[packetIndexPosition+1]<<8; 
    }
    else if(packetBlock)
    {
        uint8PacketNodePointer->index = packetBlock->PacketIndex;
    }
    else
    {
        uint8PacketNodePointer->index = 0;
    }
    
    uint8PacketNodePointer->resendCount = 0;
    uint8PacketNodePointer->resendTime = 0;
    return uint8PacketNodePointer;
}


/*��һ�����ݰ��ṹ��ڵ�ѹ���������
*Uint8PacketQueueHandle:Ҫѹ��Ĺ����ָ��
*packetBlock:���ݰ�ָ��
*
*
*/
void Uint8PacketQueuePushStruct(Uint8PacketQueue* Uint8PacketQueueHandle,PacketBlock* packetBlock)
{
    Uint8PacketNode* uint8PacketNodePointer;
    if(!Uint8PacketQueueHandle)return;
    uint8PacketNodePointer = CreatUint8PacketNode(NULL, packetBlock);
    Uint8PacketQueuePush(Uint8PacketQueueHandle, uint8PacketNodePointer);
}

/*��һ�����ݰ��ڵ�ѹ���������
*Uint8PacketQueueHandle:Ҫѹ��Ĺ����ָ��
*packet:���ݰ�ָ��
*
*
*/
void Uint8PacketQueuePushData(Uint8PacketQueue* Uint8PacketQueueHandle,uint8_t* packet)
{
    Uint8PacketNode* uint8PacketNodePointer;
    if(!Uint8PacketQueueHandle)return;
    uint8PacketNodePointer = CreatUint8PacketNode(packet, NULL);
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


/*����һ���ڵ�����
*uint8PacketNodePointer:Ҫ���͵Ľڵ�ָ�롣
*
*
*/
void SendOneCommunicationPacket(Uint8PacketNode* uint8PacketNodePointer)
{
    uint16_t protocol_PacketLength;
    uint8_t* packet;
    uint8_t packetLengthPosition;
    packetLengthPosition = sizeof(((PacketBlock*)0)->PacketHead)+sizeof(((PacketBlock*)0)->PacketIndex)+sizeof((uint8_t)(((PacketBlock*)0)->PacketFunctionWord));
    if(!uint8PacketNodePointer)return;
    packet = uint8PacketNodePointer->packet;
    protocol_PacketLength = packet[packetLengthPosition];      //��������λ��
    protocol_PacketLength += packet[packetLengthPosition+1]<<8;
    
    sendUartByteBuf(packet, protocol_PacketLength + PROTOCOL_PACKET_CONSISTENT_LENGTH);
}

/*����һ����δ���Ͷ�������
*
*
*
*/
void SendUnsentPacketQueue()
{
    Uint8PacketNode* uint8PacketNodePointer;
    while(UnsentPacketQueueHandle->head)
    {
        uint8PacketNodePointer = Uint8PacketQueuePop(UnsentPacketQueueHandle);
        SendOneCommunicationPacket(uint8PacketNodePointer);
        Uint8PacketQueuePush(UnackedPacketQueueHandle, uint8PacketNodePointer);
    }
}


/*�ͷ�һ�����ڵ�Ŀռ�
*uint8PacketNodePointer��Ҫ�ͷŵ���ڵ�ָ��
*
*/
void FreePacketNoedItem(Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer)return;
    free(uint8PacketNodePointer->packet);
    if(uint8PacketNodePointer->packetBlock)
    {
        free(uint8PacketNodePointer->packetBlock->PacketJSONData);
        free(uint8PacketNodePointer->packetBlock);
    }
    free(uint8PacketNodePointer);
}

/*ɾ�������е�һ��
*PacketQueueHandle��������ָ��
*uint8PacketNodePointer��Ҫɾ������ڵ�ָ��
*uint8PacketNodePreviousPointer��Ҫɾ�����ǰһ��Ľڵ�ָ��
*/
void DeletPacketQueueCurrentItem(Uint8PacketQueue* PacketQueueHandle, Uint8PacketNode** uint8PacketNodePointer, Uint8PacketNode** uint8PacketNodePreviousPointer)
{
    if(!*uint8PacketNodePointer)return;
    if(!*uint8PacketNodePreviousPointer)         //��һ�����  �൱��Ϊͷ�ڵ�
    {
        *uint8PacketNodePreviousPointer = *uint8PacketNodePointer;
        *uint8PacketNodePointer = (*uint8PacketNodePointer)->next;
        FreePacketNoedItem(*uint8PacketNodePreviousPointer);      
        *uint8PacketNodePreviousPointer = NULL;
        PacketQueueHandle->head = *uint8PacketNodePointer;
    }
    else
    {
        (*uint8PacketNodePreviousPointer)->next = (*uint8PacketNodePointer)->next;
        FreePacketNoedItem(*uint8PacketNodePreviousPointer);        
        *uint8PacketNodePointer = (*uint8PacketNodePreviousPointer)->next;
        if(!*uint8PacketNodePointer)         //��һ�����  �൱��Ϊβ�ڵ�
        {
            PacketQueueHandle->last = *uint8PacketNodePreviousPointer;
        }
    }
}

/*ͨ�������ж϶�����ÿһ���Ƿ�Ҫɾ��
*PacketQueueHandle��������ָ��
*PacketQueueItemCondition��һ���ж���������ָ�룬����һ���ڵ㣬���ж��Ƿ���Ҫɾ������������򷵻�True��
*
*/
void DeletPacketQueueConditionalItem(Uint8PacketQueue* PacketQueueHandle, bool (*PacketQueueItemCondition)(Uint8PacketNode* uint8PacketNodePointer))
{
    Uint8PacketNode* uint8PacketNodePointer = NULL;
    Uint8PacketNode* uint8PacketNodePreviousPointer = NULL;
    uint8PacketNodePointer = PacketQueueHandle->head;
    
    while(uint8PacketNodePointer)
    {
        if(PacketQueueItemCondition(uint8PacketNodePointer))
        {
            DeletPacketQueueCurrentItem(PacketQueueHandle, &uint8PacketNodePointer, &uint8PacketNodePreviousPointer);//��ΪҪ���ⲿ��ָ�����Ҳ����Ӱ�죬��˴˴�����Ϊָ��ָ���ָ�롣
            continue;
        }
        uint8PacketNodePreviousPointer = uint8PacketNodePointer;
        uint8PacketNodePointer = uint8PacketNodePointer->next;
    }
}

/*�ͷ�����δӦ�������������
*
*
*
*/
bool UnackedPacketAllDeletCondition(Uint8PacketNode* uint8PacketNodePointer)
{
    if(uint8PacketNodePointer)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*�������ʱ�����ط���������������ɾ������������
*
*
*
*/
bool UnackedPacketRetimeAndRecountCondition(Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer)return false;
    if(uint8PacketNodePointer->resendTime >= PROTOCOL_PACKET_RESENT_TIME_MAX)
    {
        uint8PacketNodePointer->resendTime = 0;
        SendOneCommunicationPacket(uint8PacketNodePointer);
        uint8PacketNodePointer->resendCount++;
        if(uint8PacketNodePointer->resendCount >= PROTOCOL_PACKET_RESENT_COUNT_MAX)
        {
            return true;
        }
    }
    return false;
}

/*���յ�Ӧ�����ɾ��ָ����Žڵ����������
*
*
*
*/
bool UnackedPacketAckIndexCondition(Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer)return false;
    if(uint8PacketNodePointer->index == Protocol_PacketAckedIndex)
    {
        return true;
    }
    return false;
}

/*����һ����δ��Ӧ��������
*
*
*
*/
void SendUnackedPacketQueue()
{    
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketRetimeAndRecountCondition);
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketAllDeletCondition);
}

/*����δ��Ӧ���еķ��ͼ���ʱ��
*
*
*
*/
void IncreaseUnackedPacketQueueResendTime()
{
    Uint8PacketNode* uint8PacketNodePointer = NULL;
    uint8PacketNodePointer = UnackedPacketQueueHandle->head;
    
    while(uint8PacketNodePointer)
    {
        if(uint8PacketNodePointer->resendTime < PROTOCOL_PACKET_RESENT_TIME_MAX)
        {
            uint8PacketNodePointer->resendTime++;
        }
        uint8PacketNodePointer = uint8PacketNodePointer->next;
    }
}

/*���յ�Ӧ�����ɾ��ָ����Žڵ�
*
*
*
*/
void DeleteAckedIndexPacket(uint16_t packetAckedIndex)
{
    Protocol_PacketAckedIndex = packetAckedIndex;
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketAckIndexCondition);
}

/*��װһ���ṹ���ʾ������Ϣ
*
*
*
*/
PacketBlock* AssembleCommunicationStruct(FunctionWord_TypeDef FunctionWord, uint16_t JSONMessageDataLength,uint8_t* JSONMessageData)
{
    PacketBlock* packetBlock;
    uint8_t packetHeadLength;
    packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));//����һ�����ݰ��ṹ�壬��Ϊ����ʱ�������ֽ���ʱ�ͷ�;��Ϊ����ʱ���ڴ����������ʱ���ͷ�
    packetHeadLength = sizeof(Protocol_PacketHeadData);
    memcpy(packetBlock->PacketHead,Protocol_PacketHeadData,packetHeadLength);
    packetBlock->PacketIndex = Protocol_PacketSendIndex;
    packetBlock->PacketFunctionWord = FunctionWord;
    packetBlock->PacketJSONDataLength = JSONMessageDataLength;
    packetBlock->PacketJSONData = JSONMessageData;
    
    packetBlock->PacketCheckSum = 0;
    while(packetHeadLength-- > 0)
    {
        packetBlock->PacketCheckSum += packetBlock->PacketHead[packetHeadLength];
    }
    packetBlock->PacketCheckSum += (uint8_t)(packetBlock->PacketIndex & 0x00FF);
    packetBlock->PacketCheckSum += (uint8_t)((packetBlock->PacketIndex >> 8) & 0x00FF);
    packetBlock->PacketCheckSum += (uint8_t)packetBlock->PacketFunctionWord;
    packetBlock->PacketCheckSum += (uint8_t)(packetBlock->PacketJSONDataLength & 0x00FF);
    packetBlock->PacketCheckSum += (uint8_t)((packetBlock->PacketJSONDataLength >> 8) & 0x00FF);
    while(JSONMessageDataLength-- > 0)
    {
        packetBlock->PacketCheckSum += packetBlock->PacketJSONData[JSONMessageDataLength];
    }
    
    return packetBlock;
}

/*��һ�����ṹ�����Ϊ�ַ���
*packetBlock�����ṹ��
*����ֵ���ַ���
*
*/
uint8_t* ResolvePacketStructIntoBytes(PacketBlock* packetBlock)
{
    uint8_t uint8FunctionWord;
    uint16_t protocol_PacketLength = 0;
    uint8_t* assembledPacketBuf;
    uint16_t packetBufOffset = 0;
    
    if(!packetBlock)return NULL;
    protocol_PacketLength = packetBlock->PacketJSONDataLength + PROTOCOL_PACKET_CONSISTENT_LENGTH;
    uint8FunctionWord = (uint8_t)(packetBlock->PacketFunctionWord);
    assembledPacketBuf = (uint8_t *)malloc(protocol_PacketLength * sizeof(uint8_t));//���ɴ��ֽ�����������ʱ��Unasked���г�ʱ����Ӧ����ʱ�ͷţ�������ʱ�ڴ����������ʱ���ͷš�
    
    memcpy(assembledPacketBuf + packetBufOffset,packetBlock->PacketHead,sizeof(packetBlock->PacketHead));packetBufOffset += sizeof(packetBlock->PacketHead);
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->PacketIndex),sizeof(packetBlock->PacketIndex));packetBufOffset += sizeof(packetBlock->PacketIndex);
    memcpy(assembledPacketBuf + packetBufOffset,&uint8FunctionWord,sizeof(uint8FunctionWord));packetBufOffset += sizeof(uint8FunctionWord);
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->PacketJSONDataLength),sizeof(packetBlock->PacketJSONDataLength));packetBufOffset += sizeof(packetBlock->PacketJSONDataLength);
    memcpy(assembledPacketBuf + packetBufOffset,packetBlock->PacketJSONData,packetBlock->PacketJSONDataLength);packetBufOffset += packetBlock->PacketJSONDataLength;
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->PacketCheckSum),sizeof(packetBlock->PacketCheckSum));packetBufOffset += sizeof(packetBlock->PacketCheckSum);
    
    free(packetBlock->PacketJSONData);
    free(packetBlock);
    return assembledPacketBuf;
}

/*��װһ������ѹ��δ���Ͷ���
*
*
*
*/
void AssembleProtocolPacketPushSendQueue(FunctionWord_TypeDef FunctionWord, uint16_t JSONMessageDataLength,uint8_t* JSONMessageData)
{
    uint8_t* assembledPacketBuf;
    PacketBlock* packetBlock;
    packetBlock = AssembleCommunicationStruct(FunctionWord, JSONMessageDataLength, JSONMessageData);
    assembledPacketBuf = ResolvePacketStructIntoBytes(packetBlock);
    Uint8PacketQueuePushData(UnsentPacketQueueHandle,assembledPacketBuf);
    Protocol_PacketSendIndex++;//����ŵ���
}

void PushReceiveByteDataIntoReceiveQueue(uint8_t streamByteData)//���ڷ�װ���ṩ����push��FIFO�Ľӿ�
{
    Uint8FIFOPush(ReceiveBytesFIFOQueueHandle, streamByteData);
}

void LoadQueueByteToPacketBlock(Uint8FIFOQueue* uint8FIFOQueueHandle)
{
    static bool isCommunicationPacketReceiveEnd = true;
    static PacketBlock* packetBlock = NULL;
    uint16_t count;
    
    if(isCommunicationPacketReceiveEnd == true)
    {
        if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < AT_LEAST_PACKET_BYTES_LENGTH) return;
        if(!packetBlock)packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));//ֻ�е�һ�λ�ִ��
        while(true)
        {
            if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) <= 0) return;    //���Ȳ���ʱ�˳�
            for(count=sizeof(Protocol_PacketHeadData)-1;count>1;count--)    //˳����λ
            {
                packetBlock->PacketHead[count] = packetBlock->PacketHead[count-1];
            }
            packetBlock->PacketHead[0] = Uint8FIFOPop(uint8FIFOQueueHandle);//��ȡ����ֵ
            
            for(count=0;count<sizeof(Protocol_PacketHeadData);count++)      //�Ƚ��Ƿ����
            {
                if(packetBlock->PacketHead[count] != Protocol_PacketHeadData[count])continue;
            }
            for(count=0;count<sizeof(Protocol_PacketHeadData);count++)      //���˲�˵����������ȣ���������˳�
            {
                packetBlock->PacketHead[count] = 0;
            }
            break;
        }
        if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < AT_LEAST_PACKET_BYTES_LENGTH - sizeof(Protocol_PacketHeadData)) return;
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->PacketIndex)),sizeof(((PacketBlock*)0)->PacketIndex));
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->PacketFunctionWord)),sizeof((uint8_t)(((PacketBlock*)0)->PacketFunctionWord)));
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->PacketJSONDataLength)),sizeof(((PacketBlock*)0)->PacketJSONDataLength));
    }
    if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < packetBlock->PacketJSONDataLength + sizeof(((PacketBlock*)0)->PacketCheckSum))
    {
        isCommunicationPacketReceiveEnd = false;
        return;
    }
    else
    {
        packetBlock->PacketJSONData = (uint8_t*)malloc(packetBlock->PacketJSONDataLength * sizeof(uint8_t));
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, packetBlock->PacketJSONData,packetBlock->PacketJSONDataLength);
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, &(packetBlock->PacketCheckSum),sizeof(((PacketBlock*)0)->PacketCheckSum));
        isCommunicationPacketReceiveEnd = true;
        Uint8PacketQueuePushStruct(ReceivedPacketBlockQueueHandle, packetBlock);
        packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));
        return;
    }
}

void LoadReceiveQueueByteToPacketBlock()//���ڷ�װ���ṩ�����ȡ���ؽ���FIFO���еĽӿ�
{
    LoadQueueByteToPacketBlock(ReceiveBytesFIFOQueueHandle);
}

void DealWithReceivePacketBlock(PacketBlock* packetBlock)
{
    FunctionWord_TypeDef PacketFunctionWord;
    if(!packetBlock)return;
    PacketFunctionWord = packetBlock->PacketFunctionWord;
    switch(PacketFunctionWord)
    {
        case FunctionWord_Data:
        {
            AssembleProtocolPacketPushSendQueue(packetBlock->PacketFunctionWord, packetBlock->PacketJSONDataLength, packetBlock->PacketJSONData);
        }
        break;
        
        default:
        break;
    }
}
void DealWithReceivePacketQueue()
{
    Uint8PacketNode* uint8PacketNodePointer;
    Uint8PacketNode* ReceivedPacketNodePointer;
    PacketBlock* packetBlock;
    uint8_t* assembledPacketBuf;
    
    while(true)
    {
        ReceivedPacketNodePointer = Uint8PacketQueuePop(ReceivedPacketBlockQueueHandle);
        if(!ReceivedPacketNodePointer)return;
        if(ReceivedPacketNodePointer && ReceivedPacketNodePointer->packetBlock)
        {
            if(ReceivedPacketNodePointer->packetBlock->PacketFunctionWord == FunctionWord_Acknowledgement)
            {
                DeleteAckedIndexPacket(ReceivedPacketNodePointer->index);
            }
            else
            {
                packetBlock = AssembleCommunicationStruct(FunctionWord_Acknowledgement, 0, NULL);
                packetBlock->PacketIndex = ReceivedPacketNodePointer->index;
                assembledPacketBuf = ResolvePacketStructIntoBytes(packetBlock);
                
                uint8PacketNodePointer = CreatUint8PacketNode(assembledPacketBuf, NULL);
                uint8PacketNodePointer->resendTime = PROTOCOL_PACKET_RESENT_TIME_MAX;
                uint8PacketNodePointer->resendCount = PROTOCOL_PACKET_RESENT_COUNT_MAX;
                Uint8PacketQueuePush(UnackedPacketQueueHandle, uint8PacketNodePointer);
                
                DealWithReceivePacketBlock(ReceivedPacketNodePointer->packetBlock);
                
            }
            FreePacketNoedItem(ReceivedPacketNodePointer);
        }
    }
}



