#include "CommunicationDealPacket.h"



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












