#include <stddef.h>
#include "efm32.h"
#include "NRF24.h"
#include "nRF24L01.h"
#include "efm32_usart.h"
#include "efm32_emu.h"
//#include "rtc.h"
#include "config.h"
#include "nrf24_config.h"

void NRF_Delay()
{
  volatile int i;
  for (i = 0; i < 100000; i++) {};
}
uint8_t NRF_Status(void)
{
  volatile uint8_t blah;
  NRF_CSN_lo;
  while((NRF_USART->STATUS & USART_STATUS_RXDATAV)) {
    blah = NRF_USART->RXDATA;
  }
  while(!(NRF_USART->STATUS & USART_STATUS_TXBL));
  NRF_USART->TXDATA = 0xFF;
  while (!(NRF_USART->STATUS & USART_STATUS_TXC)) ;
  NRF_CSN_hi;
  return USART_Rx(NRF_USART);
}

uint8_t NRF_ReadByte(uint8_t cmd)
{
  volatile uint8_t blah;
  NRF_CSN_lo;
  while((NRF_USART->STATUS & USART_STATUS_RXDATAV)) {
    blah = NRF_USART->RXDATA;
  }
  while(!(NRF_USART->STATUS & USART_STATUS_TXBL));
  NRF_USART->TXDATA = cmd;
  while(!(NRF_USART->STATUS & USART_STATUS_TXC));
  USART_Rx(NRF_USART);
  NRF_USART->TXDATA = 0x00;
  while (!(NRF_USART->STATUS & USART_STATUS_TXC)) ;
  NRF_CSN_hi;
  return USART_Rx(NRF_USART);
}

uint8_t NRF_ReadRegister(uint8_t reg)
{
  return NRF_ReadByte(reg | NRF_R_REGISTER);
}

void NRF_WriteRegister(uint8_t reg, uint8_t data)
{
  NRF_SendCommand((reg | NRF_W_REGISTER), data);
}

void NRF_WriteRegisterMulti(uint8_t reg, uint8_t bytes, uint8_t *data)
{
  NRF_SendPayload((NRF_W_REGISTER | reg), bytes, data);
}

void NRF_SendPayload(uint8_t reg, uint8_t bytes, uint8_t *data)
{
  NRF_CSN_lo;
  volatile uint8_t blah;
  while(!(NRF_USART->STATUS & USART_STATUS_TXBL));
  NRF_USART->TXDATA = reg;
  while(!(NRF_USART->STATUS & USART_STATUS_TXC)) ;
  blah = NRF_USART->RXDATA;
  for (int i = 0; i < bytes; i++) {
    while(!(NRF_USART->STATUS & USART_STATUS_TXBL));
	  NRF_USART->TXDATA = data[i];
    while(!(NRF_USART->STATUS & USART_STATUS_TXC)) ;
    blah = NRF_USART->RXDATA;
  }

  NRF_CSN_hi;
}

void NRF_ReceivePayload(uint8_t cmd, uint8_t bytes, uint8_t *buf)
{
  NRF_CSN_lo;
  NRF_USART->CMD = USART_CMD_CLEARRX;
  USART_Tx(NRF_USART, cmd);
  USART_Rx(NRF_USART);
  for (int i = 0; i < bytes; i++) {
	  USART_Tx(NRF_USART, 0xFF);
    buf[i] = USART_Rx(NRF_USART);
  }
  //while(!(NRF_USART->STATUS & USART_STATUS_TXC)) ;
  NRF_CSN_hi;
}


void NRF_SendCommand(uint8_t cmd, uint8_t data)
{
  NRF_CSN_lo;
  while(!(NRF_USART->STATUS & USART_STATUS_TXBL));
  NRF_USART->TXDATA = cmd;
  while(!(NRF_USART->STATUS & USART_STATUS_TXC));
  USART_Rx(NRF_USART);
  NRF_USART->TXDATA = data;
  while (!(NRF_USART->STATUS & USART_STATUS_TXC)) ;
  NRF_CSN_hi;
}

void NRF_SetupTX(void)
{
  uint8_t addr_array[5];
  NRF_CE_lo;
  NRF_WriteRegister(NRF_CONFIG, 0x0C);
  //NRF_WriteRegister(NRF_EN_AA, 0x3F); // AutoAcknowledge
  NRF_WriteRegister(NRF_EN_AA, 0x3F);
  NRF_WriteRegister(NRF_EN_RXADDR, 0x3F); // Receive Pipe 0 enabled
  NRF_WriteRegister(NRF_SETUP_RETR, 15); // Retransmits
  NRF_WriteRegister(NRF_SETUP_AW, 0x03); // Address Width  (5 bytes)
  NRF_WriteRegister(NRF_RF_SETUP, 0x0F); // RF Setup
  //NRF_WriteRegister(NRF_RF_SETUP, 0x08);  // -28dB
  NRF_WriteRegister(NRF_RF_CH, NODE_CH); // RF Channel
  NRF_WriteRegister(NRF_RX_PW_P0, 32); // RX Payload Width
  NRF_WriteRegister(NRF_FEATURE, 0x01);

#if NODE_ID == 1
  addr_array[0] = 0xE7;
  addr_array[1] = 0xE7;
  addr_array[2] = 0xE7;
  addr_array[3] = 0xE7;
  addr_array[4] = 0xE7;
#else
  addr_array[0] = 0xC0 + NODE_ID;
  addr_array[1] = 0xC2;
  addr_array[2] = 0xC2;
  addr_array[3] = 0xC2;
  addr_array[4] = 0xC2;
#endif

  NRF_WriteRegisterMulti(NRF_TX_ADDR, 5, addr_array);
  NRF_WriteRegisterMulti(NRF_RX_ADDR_P0, 5, addr_array);
  //NRF_WriteRegister(NRF_RX_ADDR_P1, 2);
  NRF_WriteRegister(NRF_STATUS, 0x7E); // Clear Interrupts
  //NRF_WriteRegister(NRF_CONFIG, 0x0E); // Power Up, Transmitter
  NRF_SendCommand(NRF_FLUSH_TX, 0xFF);
  NRF_WriteRegister(NRF_CONFIG, 0x0E);
}

void NRF_EnableRX(void)
{
  NRF_WriteRegister(NRF_STATUS, 0x7E);
  NRF_WriteRegister(NRF_CONFIG, 0x0F);
  NRF_SendCommand(NRF_FLUSH_RX, 0xFF);
  for (int i =0; i < 2; i++) {
    NRF_Delay();
  }
  NRF_CE_hi;
  for (int i =0; i < 2; i++) {
    NRF_Delay();
  }
  NRF_SendCommand(NRF_FLUSH_RX, 0xFF);
}

void CE_Lo_TriggerFunc(void)
{
  NRF_CE_lo;
}

void NRF_TransmitPacket(uint8_t bytes, uint8_t *data)
{

  NRF_WriteRegister(NRF_CONFIG, 0x0E); // Power Up, Transmitter


  NRF_SendPayload(NRF_W_TX_PAYLOAD_NOACK, bytes, data);
  NRF_CE_hi;

}


void NRF_PowerDown(void)
{
  GPIO->P[NRF_RXEN_PORT].DOUT &= ~(1 << NRF_RXEN_PIN);
  NRF_WriteRegister(NRF_CONFIG, NRF_ReadRegister(NRF_CONFIG) & (~0x02));
}

void NRF_PowerUp(void)
{
  NRF_WriteRegister(NRF_CONFIG, NRF_ReadRegister(NRF_CONFIG) | 0x02);
}

void NRF_Carrier(void)
{
  volatile int j;

  NRF_CE_lo;
  NRF_WriteRegister(NRF_CONFIG, 0x00);
  for(j = 0; j < 500000; j++);
  NRF_WriteRegister(NRF_CONFIG, 0x02);
  NRF_WriteRegister(NRF_EN_AA, 0x00);
  NRF_WriteRegister(NRF_SETUP_RETR, 0x00);
  for(j = 0; j < 5000; j++);
  NRF_WriteRegister(NRF_RF_SETUP, 0x9E);
  NRF_WriteRegister(NRF_RF_CH, NODE_CH);
  NRF_CE_hi;
}


