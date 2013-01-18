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
	if(packet.type == 1) {
	    int i;
	    int crc = 1;

	    packet.packetID = ++idT;
	    for (i = 0; i < 29; i++)
            crc ^= buff[i];
        buff[29] = crc;
    }
	if(packet.type == 0) { packet.packetID = ++idA;}
	memcpy(packet.data,buff,sizeof(packet.data));


    if(packet.type == 1){
        int id4;
        for (id4 = 0; id4 < 2; id4+=1) {
            radio_sendPacket32((uint8_t *)&packet);
            radio_loop();
            //for (volatile int i = 0; i < 10000; i++) {}
        }
    }else{
    radio_sendPacket32((uint8_t *)&packet);
}

}

int proto_receive(uint8_t* buff)
{
	int for_me = 0;

	if(radio_receivePacket32((uint8_t *)&packet))
	{
       //printf("general recive");
		if(packet.type == 0)
		{
			if (packet.packetID > idA || idA - packet.packetID > 100)
			{
				memcpy(buff,packet.data,sizeof(packet.data));
				idA = packet.packetID;
				for_me = 1;
                //printf("audio recive");
			}
		} else
		{

			if (packet.packetID > idT || idT - packet.packetID > 100) {
                int i;
                int crc = 1;
                for (i = 0; i < 29; i++)
                    crc ^= packet.data[i];
                if (packet.data[29] == crc) {
                    packet.data[29] = 0;

                    idT = packet.packetID;
                    printf("%s\n", packet.data);

                    int id4;
                    for (id4 = 0; id4 < 10; id4+=1) {
                        radio_sendPacket32((uint8_t *)&packet);
                        radio_loop();
                        //for (volatile int i = 0; i < 10000; i++) {}
                    }
                for_me = 2;
                }

            }
		}

	}
	return for_me;
}







