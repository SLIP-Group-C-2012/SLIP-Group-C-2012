#include "radio.h"
#include "efm32_int.h"
#include "protcol.h"

typedef struct 
{
	uint8_t packetID;
	uint8_t type;
	uint8_t data[30];
} Packet_Type __attribute__ ((packed));

Packet_Type packet;

uint8_t idA = 1;
uint8_t idT = 1;

void proto_send(uint8_t* buff, uint8_t type) 
{
	packet.type = type;
	if(packet.type == 1) { packet.packetID = ++idT; printf("Sending id: %d\n", idT);}
	if(packet.type == 0) { packet.packetID = ++idA;}
	memcpy(packet.data,buff,sizeof(packet.data));	
	radio_sendPacket32((uint8_t *)&packet);

}

int proto_receive(uint8_t* buff) 
{
	int for_me = 0;
		
	if(radio_receivePacket32((uint8_t *)&packet))
	{
		if(packet.type == 0)
		{
			if (packet.packetID > idA || idA - packet.packetID > 100) 
			{
				memcpy(buff,packet.data,sizeof(packet.data));
				//radio_sendPacket32((uint8_t *)&packet);
				idA = packet.packetID;
				for_me = 1;
			}
		} else
		{
			idT = packet.packetID;
			printf("PC : %s id: %d\n", packet.data, idT);
		}
		
	}
	return for_me;
}	
	
	
	
	
	
		
