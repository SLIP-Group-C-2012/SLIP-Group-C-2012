// send_receive.c
// 
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "efm32.h"
#include "efm32_chip.h"
#include "efm32_emu.h"
#include "efm32_gpio.h"
#include "efm32_i2c.h"
#include "efm32_usart.h"
#include "efm32_rtc.h"
#include "efm32_cmu.h"
#include "efm32_adc.h"
#include "efm32_timer.h"
#include "efm32_int.h"

#include "config.h"
#include "NRF24.h"
#include "nRF24L01.h"
#include "MMA845XQ.h"

#include "nrf24_config.h"

#include <math.h>

static volatile int NRF_Interrupt = 0;
static volatile int MMA_Capture = 0;

uint32_t RTCVal = 0;

uint8_t buf[192*2];

uint8_t radio_buf[32];
volatile uint8_t nrf_status = 0;

int REDval = 0;
int GREENval = 1025;
int receivedNum = 0;
int expectedNum = 1;
int lastReceived = 0;


#define SEQ_NUMBERS 88

typedef struct {
  uint8_t number;
  uint8_t padding[30];
} Packet_Type __attribute__ ((packed));

Packet_Type packet;

// sender part of the test
void sender(int num)
{
  packet.number = num;	
  printf("Send seq: %d \n", num);
  NRF_TransmitPacket(32, (uint8_t *)&packet);
   
  RXEN_hi;      
  NRF_Delay();
  NRF_EnableRX();
  RXEN_hi;  
  NRF_Delay();
}

// receiver part of the test
int receiver(void)
{
  NRF_CE_lo;
  nrf_status = NRF_ReadRegister(NRF_STATUS);
  GPIO->P[RXEN_PORT].DOUT &= ~(1 << RXEN_PIN); 
  if (nrf_status & 0x10) 
  {
    NRF_WriteRegister(NRF_STATUS, 0x10);
    //printf("nrf_status MAX_RT\n");
  } else if (nrf_status & 0x20)
  {
    NRF_WriteRegister(NRF_STATUS, 0x20);
    //printf("nrf_status TX_DS\n");
  } else if (nrf_status & 0x40)
  {
    NRF_WriteRegister(NRF_STATUS, 0x70);
    NRF_ReceivePayload(NRF_R_RX_PAYLOAD, 32, (uint8_t *)&packet);
    receivedNum = packet.number;
    printf("Received : %d -> ", receivedNum);
    NRF_SendCommand(NRF_FLUSH_RX, 0xFF);
        
    GPIO->P[RXEN_PORT].DOUT |= (1 << RXEN_PIN); 
    NRF_CE_hi;        
   }
      
   INT_Disable();
   NRF_Interrupt--;
   INT_Enable();
   
   return receivedNum;
}


void GPIO_EVEN_IRQHandler(void) 
{
  /* Acknowledge interrupt */
  if (GPIO->IF & (1 << NRF_INT_PIN)) 
  {
    NRF_Interrupt++;
    GPIO->IFC = (1 << NRF_INT_PIN);
  }
}

void GPIO_ODD_IRQHandler(void)
{
  if (GPIO->IF & (1 << MMA_INT_PIN)) 
  {
    GPIO->P[0].DOUT ^= (1 << 3);
    MMA_Capture++;
    //RTCVal = RTC_CounterGet();
    GPIO->IFC = (1 << MMA_INT_PIN);
  }
}

void LETIMER0_IRQHandler(void)
{
}
void TIMER0_IRQHandler(void)
{ 
  TIMER_CompareBufSet(TIMER0, 0, REDval);
  TIMER_CompareBufSet(TIMER0, 1, GREENval);
  
  TIMER_IntClear(TIMER0, TIMER_IF_OF);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{  
  /* Chip errata */
  CHIP_Init();
  /* Ensure core frequency has been updated */
  SystemCoreClockUpdate();

  IO_Init();
  UART1->ROUTE = UART_ROUTE_LOCATION_LOC3
          | UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;
  
  TIMER_IntClear(TIMER0, TIMER_IF_OF);
  InitRGBLEDPWM();
  TIMER_IntClear(TIMER0, TIMER_IF_OF);
  uart_init(UART1); // for printf
  
  printf("Start COM\n");

  NRF_SetupTX(); // set up radio

  printf("Enabling Radio\n");
  
  NRF_WriteRegister(NRF_STATUS, 0x70); // Clear all radio interrupts

  while(1)
  { 
    while (NRF_Interrupt>0) 
    {
      receivedNum = receiver();
      lastReceived = receivedNum;
    }
    // remove the (+1) for one of the devices
    sender(lastReceived+1);
  }
}

