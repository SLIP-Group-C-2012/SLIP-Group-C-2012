#ifndef __NRF24_CONFIG_H
#define __NRF24_CONFIG_H
#include "config.h"

#ifdef RESP_DEVICE
#define NRF_CE_PORT 2
#define NRF_CE_PIN 15
#define NRF_INT_PORT 2
#define NRF_INT_PIN 14
#define NRF_CSN_PORT 4
#define NRF_CSN_PIN 13
#define NRF_USART USART0
#elif defined RESP_PRESSURE
#define NRF_CE_PORT 2
#define NRF_CE_PIN 15
#define NRF_INT_PORT 2
#define NRF_INT_PIN 14
#define NRF_CSN_PORT 4
#define NRF_CSN_PIN 13
#define NRF_USART USART0
#elif defined RESP_DEVKIT
#define NRF_CE_PORT 3
#define NRF_CE_PIN 4
#define NRF_INT_PORT 3
#define NRF_INT_PIN 5
#define NRF_CSN_PORT 3
#define NRF_CSN_PIN 3
#define NRF_USART USART1
#elif defined RESP_BLACK
#define NRF_CE_PORT 1
#define NRF_CE_PIN 11
#define NRF_INT_PORT 2
#define NRF_INT_PIN 14
#define NRF_CSN_PORT 4
#define NRF_CSN_PIN 13
#define NRF_USART USART0
#elif defined PROSP_H
#define NRF_CE_PORT 1
#define NRF_CE_PIN 1
#define NRF_INT_PORT 1
#define NRF_INT_PIN 0
#define NRF_CSN_PORT 1
#define NRF_CSN_PIN 2
#define NRF_RXEN_PORT 0
#define NRF_RXEN_PIN 2
#define NRF_USART USART2
#endif

#define NRF_CSN_hi GPIO->P[NRF_CSN_PORT].DOUTSET = (1 << NRF_CSN_PIN);
#define NRF_CSN_lo GPIO->P[NRF_CSN_PORT].DOUTCLR = (1 << NRF_CSN_PIN);
#define NRF_CE_hi GPIO->P[NRF_CE_PORT].DOUTSET = (1 << NRF_CE_PIN);
#define NRF_CE_lo GPIO->P[NRF_CE_PORT].DOUTCLR = (1 << NRF_CE_PIN);
#define RXEN_hi GPIO->P[RXEN_PORT].DOUTSET = (1 << RXEN_PIN);
#define RXEN_lo GPIO->P[RXEN_PORT].DOUTCLR = (1 << RXEN_PIN);


#endif // __NRF24_CONFIG_H
