#include "radio.h"
#include "efm32_int.h"
#include "protcol.h"

// Please set MY_ADDR to 2 for the speaker board
#define MY_ADDR (1)

typedef struct 
{
	uint8_t src;
	uint8_t dest;
	uint8_t packetID;
	uint8_t data[29];
} Packet_Type __attribute__ ((packed));

Packet_Type packet;

int id = 1;

int lastReceived = 0;
//int received_count = 0;
/*
uint8_t test_buffer = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28};
uint8_t test_end = {0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28};
uint8_t test_receive[28];
*/

void proto_send(uint8_t* buff, int dest) 
{
	//printf("Sending packet from %d to %d\n", MY_ADDR, dest);
	packet.src = MY_ADDR;
	packet.dest = dest;
	packet.packetID = ++id;
	memcpy(packet.data,buff,sizeof(packet.data));

	radio_sendPacket32((uint8_t *)&packet);

}

int proto_receive(uint8_t* buff) 
{
	int for_me = 0;
	if(radio_receivePacket32((uint8_t *)&packet))
	{
		if(packet.dest == MY_ADDR)
		//Received packet for me
			memcpy(buff,packet.data,sizeof(packet.data));
			for_me = 1;
		} else if(packet.source != MY_ADDR)
		//Retransmission
		{
				radio_sendPacket32((uint8_t *)&packet);
		}
	}
	return for_me;
}
/*	
void test_sender(int counter)
{	
	for(int i = 0; i < counter; i++)
	{
		printf("Sending counter = %d\n", counter);
		proto_send(test_buffer, 2);
	}
	printf("Sent %d packets.\n", counter);
}

void test_receiver() 
{
	proto_receive(test_receive);
	if(test_receive[0] == 0)
	{
		received_count++;
		printf("END : %d", received_count);
	} else {
		received_count++;
	}	
}
*/	
	
	
	
	
	
	
	
	
	
	
	
	
		
