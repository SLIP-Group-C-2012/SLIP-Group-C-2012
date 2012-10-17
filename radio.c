#include "NRF24.h"
#include "nRF24L01.h"
#include "efm32_int.h"
#include "nrf24_config.h"

#define MODE_RECEIVE (1)
#define MODE_SEND (2)

#define MAX_LEGAL_CHANNEL (90) // maximal allowed channel

uint8_t radio_mode;

static volatile int NRF_Interrupt = 0;

void toStandByI(void)
{
    NRF_CE_lo;
    NRF_Delay(); // IS THIS NECESSARY?
}

void receiveModeEnable(void)
{
    NRF_Delay(); // give enough time to TX last packet if there is one

    toStandByI(); // return to standby

    NRF_WriteRegister(NRF_STATUS, 0x7E); // reset status register
    NRF_SendCommand(NRF_FLUSH_RX, 0xFF); // Flush RX FIFO
    NRF_WriteRegister(NRF_CONFIG, 0x0F); // PRIM_RX HI & PWR_UP
    NRF_CE_hi;

    NRF_Delay();

    radio_mode = MODE_RECEIVE;
}

void toStandByII(void)
{
    toStandByI(); // return to standby

    NRF_WriteRegister(NRF_STATUS, 0x7E); // reset status register
    NRF_WriteRegister(NRF_CONFIG, 0x0E); // PRIM_RX LO & PWR_UP
    NRF_CE_hi;
    NRF_SendCommand(NRF_FLUSH_TX, 0xFF); // Flush TX FIFO

    radio_mode = MODE_SEND;
}

void radio_handleInterrupt(void)
{
    if (GPIO->IF & (1 << NRF_INT_PIN))
    {
        NRF_Interrupt++;
        GPIO->IFC = (1 << NRF_INT_PIN);
    }
}

void radio_setup(uint8_t channel, uint8_t bandwidth, uint8_t power)
{
    uint8_t addr_array[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};

    RXEN_hi; // enable amp

    if (channel > MAX_LEGAL_CHANNEL) return; // keeps police happy

    NRF_CE_lo;
    NRF_WriteRegister(NRF_CONFIG, 0x3C); // Enable CRC and disable TX interrupts
    NRF_WriteRegister(NRF_EN_AA, 0x0); // Disable auto ACK
    NRF_WriteRegister(NRF_EN_RXADDR, 0x3F); // Receive Pipe 0 enabled
    NRF_WriteRegister(NRF_SETUP_RETR, 0); // Retransmits
    NRF_WriteRegister(NRF_SETUP_AW, 0x03); // Address Width  (5 bytes)
    NRF_WriteRegister(NRF_RF_SETUP, bandwidth | power); // set up
    NRF_WriteRegister(NRF_RF_CH, channel); // RF Channel
    NRF_WriteRegister(NRF_RX_PW_P0, 32); // RX Payload Width
    NRF_WriteRegister(NRF_FEATURE, 0x01);

    NRF_WriteRegisterMulti(NRF_TX_ADDR, 5, addr_array);
    NRF_WriteRegisterMulti(NRF_RX_ADDR_P0, 5, addr_array);
    NRF_WriteRegister(NRF_STATUS, 0x7E); // Clear Interrupts
    NRF_SendCommand(NRF_FLUSH_TX, 0xFF);
    NRF_WriteRegister(NRF_CONFIG, 0x0E);

    receiveModeEnable();

    NRF_WriteRegister(NRF_STATUS, 0x70); // clear radio interrupts
}

int packet_wait = 0;

void radio_sendPacket(uint8_t * data, uint8_t size)
{
    volatile int j;

    while (packet_wait) {};

    packet_wait = 1;

    //if (radio_mode != MODE_SEND)
    //    toStandByII();

    NRF_SendPayload(NRF_W_TX_PAYLOAD_NOACK, size, data);

    NRF_CE_hi;
    for (j = 0; j < 500; j++) {};
    NRF_CE_lo;

}

uint8_t radio_receivePacket(uint8_t * data, uint8_t size)
{
    uint8_t read = 0;
    uint8_t nrf_status;

    while (NRF_Interrupt>0)
    {

        NRF_CE_lo;
        nrf_status = NRF_ReadRegister(NRF_STATUS);
        RXEN_lo;

        if (nrf_status & 0x10)
        {
            NRF_WriteRegister(NRF_STATUS, 0x10);
            //printf("nrf_status MAX_RT\n");
        } else if (nrf_status & 0x20)
        {
            NRF_WriteRegister(NRF_STATUS, 0x20);
            printf(" OK\n");

            if (radio_mode != MODE_RECEIVE)
                receiveModeEnable();

            packet_wait = 0;
        } else if (nrf_status & 0x40)
        {
            NRF_WriteRegister(NRF_STATUS, 0x70);
            NRF_ReceivePayload(NRF_R_RX_PAYLOAD, size, data);
            read = 1;

            NRF_SendCommand(NRF_FLUSH_RX, 0xFF);

        } else printf("Interrupt 0x%x\n", nrf_status);

        NRF_CE_hi;
        RXEN_hi;

        INT_Disable();
        NRF_Interrupt--;
        INT_Enable();

        if (read) continue;
    }

    return read;
}
