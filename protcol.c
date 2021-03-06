#include "radio.h"
//#include "NRF24.h"
//#include "nRF24L01.h"
#include "efm32_int.h"
#include "protcol.h"
//#include "nrf24_config.h"
//#define NULL ((void*)0)

#define DISABLE_REBROADCAST (1)


uint8_t packetbuff[32];
int packetpending = 0;
uint8_t addressbook[10];
uint8_t pingdata[32] = "PING";


typedef struct {
	uint8_t opcode; // opcode 1 = text, 2 = audio 3, = rip
	uint8_t src;
	uint8_t dest;
	uint8_t packetID;
	uint8_t hops;
	uint8_t data[32-5];
} Packet_Type __attribute__ ((packed));

Packet_Type packet;
int recivedupto = 0;

void protocol_send(uint8_t* buff, uint8_t dest) {
	packet.opcode = 1; // opcode 1 = text, 2 = audio 3, = rip
	packet.src = protocol_getaddr();
	packet.dest = 0;
	recivedupto ++;
	packet.packetID = recivedupto;
	packet.hops = 0;
	memcpy(packet.data,buff,sizeof(packet.data));
	radio_sendPacket32((uint8_t *)&packet);
}

int protocol_recive(uint8_t* buff){
	if (packetpending){
		//printf("String1 %s\n",packetbuff);

		//printf("String2 %s\n",test);
		if(!strcmp(packetbuff,pingdata)){
			//printf("yes");
			protocol_updateaddr(packet.src);
		}

		memcpy(packetbuff,buff,sizeof(packetbuff));
		packetpending = 0;
		return 1;
	}
	return 0;
}


int protocol_loop(){
	radio_loop();
	int forus = 0;
	if(radio_receivePacket32((uint8_t *)&packet))
	{

		if(recivedupto<packet.packetID){
			recivedupto = packet.packetID;

			memcpy(packetbuff,packet.data,sizeof(packet.data));
			forus = 1;
			packetpending = 1;

        if (!DISABLE_REBROADCAST) {
			if (packet.hops<=4){
				packet.hops++;
				radio_sendPacket32((uint8_t *)&packet);
				printf("Rebroadcast");
			}
		}
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

void protocol_updateaddr(uint8_t address){

	//printf("update, %i \n",address);

	//addressbook[2] = address;

	int alreadyinbook = 0;
	for(int i = 0; i < 10; i++)
	{
		if(addressbook[i]==address){
			alreadyinbook = 1;
			//printf("foundat:%i\n",i);
			break;
		}
	}
	if(!alreadyinbook){
		//printf("!already");
		for(int i = 0; i < 10; i++){
			if(addressbook[i]==0){
				addressbook[i] = address;
				alreadyinbook  = 0;
				break;
			}
		}

	}

}

void protocol_printalladdr(void){
	for(int i = 0; i < 10; i++)
	{
		if(!strcmp(addressbook[i],0)){
			printf("Address %d\n",addressbook[i]);
		}
	}
}

