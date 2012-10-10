#ifndef __NRF24_H
#define __NRF24_H

#include "efm32.h"

uint8_t NRF_Status(void);
uint8_t NRF_ReadByte(uint8_t cmd);
uint8_t NRF_ReadRegister(uint8_t reg);
void NRF_WriteRegister(uint8_t reg, uint8_t data);
void NRF_WriteRegisterMulti(uint8_t reg, uint8_t bytes, uint8_t *data);
void NRF_SendPayload(uint8_t reg, uint8_t bytes, uint8_t *data);
void NRF_SendCommand(uint8_t cmd, uint8_t data);
void NRF_SetupTX(void);
void NRF_EnableRX(void);
void NRF_Carrier(void);
void NRF_TransmitPacket(uint8_t bytes, uint8_t *data);
void NRF_PowerDown(void);
void NRF_PowerUp(void);

#endif /* __NRF24_H	*/
