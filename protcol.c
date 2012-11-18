#include "radio.h"
//#include "NRF24.h"
//#include "nRF24L01.h"
#include "efm32_int.h"
#include "protcol.h"
//#include "nrf24_config.h"
//#define NULL ((void*)0)

#define DISABLE_REBROADCAST (1)

#define MY_ADDR (1)


uint8_t packetbuff[29];
int packetpending = 0;

typedef struct {
	uint8_t src;
	uint8_t dest;
	uint8_t packetID;
	uint8_t data[29];
} Packet_Type __attribute__ ((packed));

Packet_Type packet;

int id = 1;

void protocol_send(uint8_t* buff, uint8_t dest) {
	packet.src = protocol_getaddr();
	packet.dest = dest;
	packet.packetID = ++id;
	memcpy(packet.data,buff,sizeof(packet.data));
	radio_sendPacket32((uint8_t *)&packet);
}

int protocol_recive(uint8_t* buff){	

	if (packetpending){
		//printf("Copiyng %d bytes ->>> buff\n", sizeof(packetbuff));
		memcpy(buff,packetbuff,sizeof(packetbuff));
		packetpending = 0;
		return 1;
	}
	return 0;
}


int protocol_loop(){
	radio_loop();
	int forus = 0;
	int lastReceived = 0;
	if(radio_receivePacket32((uint8_t *)&packet))
	{
		//printf("SRC: %d DEST: %d ID: %d \n" , packet.src, packet.dest, packet.packetID);
		if(packet.dest == protocol_getaddr())
		{
			//printf("12");
			forus = 1;
			packetpending = 1;
			lastReceived = packet.packetID;
			//printf("Received %d bytes ->>> packetbuff\n", sizeof(packet.data));
			memcpy(packetbuff,packet.data,sizeof(packet.data));
		} else if(!DISABLE_REBROADCAST) {
			forus = 0;
			packetpending = 0;
			//printf("Retransimit\n");
			packet.src = protocol_getaddr();
			radio_sendPacket32((uint8_t *)&packet);
		}
	}
	return forus;
}

uint8_t protocol_getaddr(){
	uint32_t serial1;
	serial1 =  (*(uint32_t*)0x0FE081F0) ^ (*(uint32_t*)0x0FE081F4); //UNIQUE_0 is a timestamp (in Unix time) set in production,
	uint8_t * addr = &serial1;
	return addr[0] ^ addr[1] ^ addr[2] ^ addr[3];
}

