#include "NRF24.h"
#include "nRF24L01.h"
#include "efm32_int.h"

#define MODE_RECEIVE (1)
#define MODE_SEND (2)

#include "nrf24_config.h"

uint8_t radio_mode;

static volatile int NRF_Interrupt = 0;

void receiveModeEnable(void)
{

    RXEN_hi;

    NRF_Delay();
    NRF_EnableRX();
    RXEN_hi;
    NRF_Delay();

    //NRF_WriteRegister(NRF_STATUS, 0x70); // clear radio interrupts
    //radio_mode = MODE_RECEIVE;
}

void sendModeEnable(void)
{
    //NRF_CE_lo;
    //RXEN_lo;

    //NRF_Delay();

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

void radio_setup(uint8_t channel, uint8_t speed)
{
    NRF_SetupTX();

    NRF_WriteRegister(NRF_STATUS, 0x70); // clear radio interrupts
    radio_mode = MODE_RECEIVE;
    //receiveModeEnable();
}

void radio_sendPacket(uint8_t * data, uint8_t size)
{
    if (radio_mode != MODE_SEND)
        sendModeEnable();

    NRF_TransmitPacket(size, data);
    RXEN_hi;
}

uint8_t radio_receivePacket(uint8_t * data, uint8_t size)
{
    uint8_t read = 0;
    uint8_t nrf_status;

    if (radio_mode != MODE_RECEIVE)
        receiveModeEnable();

    while (NRF_Interrupt>0)
    {

        NRF_CE_lo;
        nrf_status = NRF_ReadRegister(NRF_STATUS);
        GPIO->P[RXEN_PORT].DOUT &= ~(1 << RXEN_PIN);

        if (nrf_status & 0x10)
        {
            NRF_WriteRegister(NRF_STATUS, 0x10);
            printf("nrf_status MAX_RT\n");
        }
        else if (nrf_status & 0x20)
        {
            NRF_WriteRegister(NRF_STATUS, 0x20);
            printf("nrf_status TX_DS\n");
        }
        else if (nrf_status & 0x40)
        {
            printf("nrf_status DATA\n");
            NRF_WriteRegister(NRF_STATUS, 0x70);
            NRF_ReceivePayload(NRF_R_RX_PAYLOAD, size, data);
            read = 1;

            NRF_SendCommand(NRF_FLUSH_RX, 0xFF);

            GPIO->P[RXEN_PORT].DOUT |= (1 << RXEN_PIN);
            NRF_CE_hi;
        }
        else printf("Stat %x\n", nrf_status);

        INT_Disable();
        NRF_Interrupt--;
        INT_Enable();

        if (read) break;
    }

    return read;
}
