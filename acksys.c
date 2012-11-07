#include "radio.h"

#define OPCODE_DATA (0)
#define OPCODE_ACK (1)

#define BUFFERED_PACKETS (10)
#define MY_ADDRESS (0)

typedef struct {
    uint8_t opcode;
    uint8_t packetID;
    uint8_t src;
    uint8_t dest;
    uint8_t data[28];
} Packet_Type __attribute__ ((packed));

Packet_Type packet;

Packet_Type packets[BUFFERED_PACKETS];
int packets_id = 0;

int packetID = 0;

void acksys_sendData(uint8_t * data, uint8_t dest) {
    packet.opcode = OPCODE_DATA;
    packet.packetID = packetID++;
    packet.src = MY_ADDRESS;
    packet.dest = dest;
    memcpy(packet.data, data, sizeof(packet.data));

    radio_sendPacket32((uint8_t *) &packet);

    if (packets_id == BUFFERED_PACKETS)
        packets_id = 0;
    memcpy(&packets[packets_id++], &packet, sizeof(packet));
}

int acksys_recvData(uint8_t * data)
{
    if (radio_receivePacket32(data))
    {
        if (packet.dest == MY_ADDRESS)
        {
            switch (packet.opcode)
            {
                int isok, i;

                case OPCODE_DATA:

                    isok = 0;

                    if (packet.packetID == packetID + 1)
                    {
                        packetID ++;

                        memcpy(data, packet.data, sizeof(packet.data));
                        isok = 1;
                    }

                    packet.opcode = OPCODE_ACK;
                    packet.packetID = packetID;
                    packet.dest = packet.src;
                    packet.src = MY_ADDRESS;

                    radio_sendPacket32((uint8_t *) &packet);

                return isok;

                case OPCODE_ACK:

                    for (i = 0; i < BUFFERED_PACKETS; i++)
                        if (packets[i].packetID == packetID)
                        {
                            radio_sendPacket32((uint8_t *) &packets[i]);
                            return 0;
                        }

                    printf("END OF WORLD\n");

                break;
                default:
                    printf("Unknown opcode");
                break;
            }
        }
    }

    return 0;
}
