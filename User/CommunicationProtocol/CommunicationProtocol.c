#include "CommunicationProtocol.h"


#define AT_LEAST_PACKET_BYTES_LENGTH 8

const uint8_t Protocol_headData[2] = {0xAA,0xAA};
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





/*����һ�����Ͱ��Ķ���
*����һ���������ָ��
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
Uint8PacketNode* CreatUint8PacketNode(uint8_t* packet, PacketBlock* packetBlock)
{
    Uint8PacketNode* uint8PacketNodePointer;
    uint8_t packetIndexPosition;
    packetIndexPosition = sizeof(((PacketBlock*)0)->head);
    
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
        uint8PacketNodePointer->index = packetBlock->index;
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
void Uint8PacketQueuePushBlock(Uint8PacketQueue* Uint8PacketQueueHandle,PacketBlock* packetBlock)
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
void Uint8PacketQueuePushStreamData(Uint8PacketQueue* Uint8PacketQueueHandle,uint8_t* packet)
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
*/
void SendOneCommunicationPacketNode(Uint8PacketNode* uint8PacketNodePointer)
{
    uint16_t protocol_PacketLength;
    uint8_t* packet;
    uint8_t packetLengthPosition;
    packetLengthPosition = sizeof(((PacketBlock*)0)->head)+sizeof(((PacketBlock*)0)->index)+sizeof((uint8_t)(((PacketBlock*)0)->functionWord));
    if(!uint8PacketNodePointer)return;
    packet = uint8PacketNodePointer->packet;
    protocol_PacketLength = packet[packetLengthPosition];      //��������λ��
    protocol_PacketLength += packet[packetLengthPosition+1]<<8;
    
    sendUartByteBuf(packet, protocol_PacketLength + PROTOCOL_PACKET_CONSISTENT_LENGTH);
}

/*����һ����δ���Ͷ�������
*
*/
void SendUnsentPacketQueue()
{
    Uint8PacketNode* uint8PacketNodePointer;
    while(UnsentPacketQueueHandle->head)
    {
        uint8PacketNodePointer = Uint8PacketQueuePop(UnsentPacketQueueHandle);
        SendOneCommunicationPacketNode(uint8PacketNodePointer);
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
    if(uint8PacketNodePointer->packet)free(uint8PacketNodePointer->packet);
    if(uint8PacketNodePointer->packetBlock)
    {
        if(uint8PacketNodePointer->packetBlock->messageData)free(uint8PacketNodePointer->packetBlock->messageData);
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
*/
bool UnackedPacketRetimeAndRecountCondition(Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer)return false;
    if(uint8PacketNodePointer->resendTime >= PROTOCOL_PACKET_RESENT_TIME_MAX)
    {
        uint8PacketNodePointer->resendTime = 0;
        SendOneCommunicationPacketNode(uint8PacketNodePointer);
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
*/
void SendUnackedPacketQueue()
{    
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketRetimeAndRecountCondition);
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketAllDeletCondition);
}

/*����δ��Ӧ���еķ��ͼ���ʱ��
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
*/
void DeleteAckedIndexPacket(uint16_t packetAckedIndex)
{
    Protocol_PacketAckedIndex = packetAckedIndex;
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketAckIndexCondition);
}
/*����һ�����ݿ��У���
*
*/
uint8_t CalculatePacketBlockCheckSum(PacketBlock* packetBlock)
{
    uint8_t headLength;
    uint16_t MessageDataLength;
    uint8_t checkSum = 0;
    if(!packetBlock)return NULL;
    
    MessageDataLength = packetBlock->messageDataLength;
    headLength = sizeof(Protocol_headData);
    while(headLength-- > 0)
    {
        checkSum += packetBlock->head[headLength];
    }
    checkSum += (uint8_t)(packetBlock->index & 0x00FF);
    checkSum += (uint8_t)((packetBlock->index >> 8) & 0x00FF);
    checkSum += (uint8_t)packetBlock->functionWord;
    checkSum += (uint8_t)(packetBlock->messageDataLength & 0x00FF);
    checkSum += (uint8_t)((packetBlock->messageDataLength >> 8) & 0x00FF);
    while(MessageDataLength-- > 0)
    {
        checkSum += packetBlock->messageData[MessageDataLength];
    }
    return checkSum;
}

/*��װһ���ṹ���ʾ������Ϣ
*
*/
PacketBlock* AssembleProtocolPacketBlock(FunctionWord_TypeDef FunctionWord, uint16_t MessageDataLength,uint8_t* MessageData)
{
    PacketBlock* packetBlock;
    packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));//����һ�����ݰ��ṹ�壬��Ϊ����ʱ�������ֽ���ʱ�ͷ�;��Ϊ����ʱ���ڴ����������ʱ���ͷ�
    memcpy(packetBlock->head,Protocol_headData,sizeof(Protocol_headData));
    packetBlock->index = Protocol_PacketSendIndex;
    packetBlock->functionWord = FunctionWord;
    packetBlock->messageDataLength = MessageDataLength;
    packetBlock->messageData = MessageData;
    
    packetBlock->checkSum = CalculatePacketBlockCheckSum(packetBlock);
    
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
    if(!packetBlock->messageData){free(packetBlock);return NULL;}
    protocol_PacketLength = packetBlock->messageDataLength + PROTOCOL_PACKET_CONSISTENT_LENGTH;
    uint8FunctionWord = (uint8_t)(packetBlock->functionWord);
    assembledPacketBuf = (uint8_t *)malloc(protocol_PacketLength * sizeof(uint8_t));//���ɴ��ֽ�����������ʱ��Unasked���г�ʱ����Ӧ����ʱ�ͷţ�������ʱ�ڴ����������ʱ���ͷš�
    
    memcpy(assembledPacketBuf + packetBufOffset,packetBlock->head,sizeof(packetBlock->head));packetBufOffset += sizeof(packetBlock->head);
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->index),sizeof(packetBlock->index));packetBufOffset += sizeof(packetBlock->index);
    memcpy(assembledPacketBuf + packetBufOffset,&uint8FunctionWord,sizeof(uint8FunctionWord));packetBufOffset += sizeof(uint8FunctionWord);
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->messageDataLength),sizeof(packetBlock->messageDataLength));packetBufOffset += sizeof(packetBlock->messageDataLength);
    memcpy(assembledPacketBuf + packetBufOffset,packetBlock->messageData,packetBlock->messageDataLength);packetBufOffset += packetBlock->messageDataLength;
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->checkSum),sizeof(packetBlock->checkSum));packetBufOffset += sizeof(packetBlock->checkSum);
    
    free(packetBlock->messageData);
    free(packetBlock);
    return assembledPacketBuf;
}

/*��װһ������ѹ��δ���Ͷ���
*
*
*/
void AssembleProtocolPacketPushSendQueue(FunctionWord_TypeDef FunctionWord, uint16_t MessageDataLength,uint8_t* MessageData)
{
    uint8_t* assembledPacketBuf;
    PacketBlock* packetBlock;
    packetBlock = AssembleProtocolPacketBlock(FunctionWord, MessageDataLength, MessageData);
    assembledPacketBuf = ResolvePacketStructIntoBytes(packetBlock);
    Uint8PacketQueuePushStreamData(UnsentPacketQueueHandle,assembledPacketBuf);
    Protocol_PacketSendIndex++;//����ŵ���
}
/*���ڷ�װ���ṩ����push������FIFO�Ľӿ�
*
*/
void PushReceiveByteDataIntoReceiveFIFO(uint8_t streamByteData)//���ڷ�װ���ṩ����push��FIFO�Ľӿ�
{
    Uint8FIFOPush(ReceiveBytesFIFOQueueHandle, streamByteData);
}
/*��ֱ����FIFO�н���ɿ鲢��ӵ����հ�����
*
*
*/
void LoadQueueByteToPacketBlock(Uint8FIFOQueue* uint8FIFOQueueHandle)
{
    static bool isCommunicationPacketReceiveEnd = true;
    static PacketBlock* packetBlock = NULL;
    uint16_t count;
    bool isHeadAllEqual;
    
    while(true)
    {
        if(isCommunicationPacketReceiveEnd == true)
        {
            if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < AT_LEAST_PACKET_BYTES_LENGTH) return;
            if(!packetBlock)packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));//ֻ�е�һ�λ�ִ��
            while(true)
            {
                if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) <= 0) return;    //���Ȳ���ʱ�˳�     
                for(count=sizeof(Protocol_headData)-1;count>0;count--)    //˳����λ
                {
                    packetBlock->head[count] = packetBlock->head[count-1];
                }
                packetBlock->head[0] = Uint8FIFOPop(uint8FIFOQueueHandle);//��ȡ����ֵ
          
                isHeadAllEqual = true;
                for(count=0;count<sizeof(Protocol_headData);count++)      //�Ƚ��Ƿ����
                {
                    if(packetBlock->head[count] != Protocol_headData[count])isHeadAllEqual=false;
                }
                if(isHeadAllEqual)
                {
                    break;
                }
            } 
            
            if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < AT_LEAST_PACKET_BYTES_LENGTH - sizeof(Protocol_headData)) return;
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->index)),sizeof(((PacketBlock*)0)->index));
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->functionWord)),sizeof((uint8_t)(((PacketBlock*)0)->functionWord)));
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->messageDataLength)),sizeof(((PacketBlock*)0)->messageDataLength));
            
        }
        if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < packetBlock->messageDataLength + sizeof(((PacketBlock*)0)->checkSum))
        {
            isCommunicationPacketReceiveEnd = false;
            return;
        }
        else
        {
            packetBlock->messageData = (uint8_t*)malloc(packetBlock->messageDataLength * sizeof(uint8_t));
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, packetBlock->messageData,packetBlock->messageDataLength);
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, &(packetBlock->checkSum),sizeof(((PacketBlock*)0)->checkSum));
            isCommunicationPacketReceiveEnd = true;
            if(packetBlock->checkSum != CalculatePacketBlockCheckSum(packetBlock))
            {
                free(packetBlock->messageData);
                free(packetBlock);
                printf("Bad block\r\n");
            }
            else
            {
                Uint8PacketQueuePushBlock(ReceivedPacketBlockQueueHandle, packetBlock);
            }
            packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));
            packetBlock->head[0]=packetBlock->head[1]=0;
            return;
        }
    }
}
/*���ڷ�װ���ṩ�����ȡ���ؽ���FIFO���еĽӿ�
*
*
*/
void LoadReceiveQueueByteToPacketBlock()
{
    LoadQueueByteToPacketBlock(ReceiveBytesFIFOQueueHandle);
}
/*�����ݿ���д���
*
*
*/
void DealWithReceivePacketBlock(PacketBlock* packetBlock)
{
    if(!packetBlock)return;
    switch(packetBlock->functionWord)
    {
        case FunctionWord_Data:
        {
            printf("%d\t%d\r\n",Protocol_PacketSendIndex++,packetBlock->messageDataLength);
            Uint8PacketQueuePushStreamData(UnsentPacketQueueHandle,ResolvePacketStructIntoBytes(packetBlock));
        }
        break;
        
        default:
            free(packetBlock->messageData);
            free(packetBlock);
        break;
    }
}
/*�Խ��հ�����ÿ�����ݿ���д���
*
*
*/
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
            if(ReceivedPacketNodePointer->packetBlock->functionWord == FunctionWord_Acknowledgement)
            {
                DeleteAckedIndexPacket(ReceivedPacketNodePointer->index);
            }
            else
            {
                /**���ͻ�Ӧ��**/
//                packetBlock = AssembleProtocolPacketBlock(FunctionWord_Acknowledgement, 0, NULL);
//                packetBlock->index = ReceivedPacketNodePointer->index;
//                assembledPacketBuf = ResolvePacketStructIntoBytes(packetBlock);
//                
//                uint8PacketNodePointer = CreatUint8PacketNode(assembledPacketBuf, NULL);
//                uint8PacketNodePointer->resendTime = PROTOCOL_PACKET_RESENT_TIME_MAX;
//                uint8PacketNodePointer->resendCount = PROTOCOL_PACKET_RESENT_COUNT_MAX;
//                Uint8PacketQueuePush(UnackedPacketQueueHandle, uint8PacketNodePointer);
                
                packetBlock = ReceivedPacketNodePointer->packetBlock;
                ReceivedPacketNodePointer->packetBlock = NULL;
                DealWithReceivePacketBlock(packetBlock); 
            }
        }
        FreePacketNoedItem(ReceivedPacketNodePointer);
    }
}



