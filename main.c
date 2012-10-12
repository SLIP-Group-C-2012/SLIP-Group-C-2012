/**************************************************************************//**
 * @file
 * @brief Simple LCD blink demo for EFM32_Gxxx_STK
 * @author Energy Micro AS
 * @version 2.1.2
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2009 Energy Micro AS, http://www.energymicro.com</b>
 ******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/
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

Accel_Vector_Type accelReading;

#define PACKET_TYPE_ACCELDATA 1

typedef struct {
  uint8_t type;
  uint8_t nodeID;
  int16_t accelX;
  int16_t accelY;
  int16_t accelZ;
  uint8_t padding[24];
} Packet_Type __attribute__ ((packed));

Packet_Type packet;

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
  //REDval = (REDval + 1) % 1025;
  //GREENval = (GREENval - 1) % 1025;
//  if(GPIO->P[NRF_INT_PORT].DIN & (1 << NRF_INT_PIN)) {
//    GPIO->P[LED_GREEN_PORT].DOUT &= ~(1 << LED_GREEN_PIN);
//    GPIO->P[LED_RED_PORT].DOUT |= (1 << LED_RED_PIN);
//  } else {
//    GPIO->P[LED_RED_PORT].DOUT &= ~(1 << LED_RED_PIN);
//    GPIO->P[LED_GREEN_PORT].DOUT |= (1 << LED_GREEN_PIN);
//  }
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
  
  printf("Hello World\n");

  #ifndef BASESTATION
  MMAInit(); // set up accelerometer
  MMARegReadN(OUT_X_MSB_REG, 6, buf);
  #endif // !BASESTATION

  NRF_SetupTX(); // set up radio

  #ifdef BASESTATION
  NRF_EnableRX();
  RXEN_hi;
  #endif // BASESTATION
  
  NRF_WriteRegister(NRF_STATUS, 0x70); // Clear all radio interrupts


  //NRF_PowerDown(); // power down radio until we need it to save power

  while(1)
  {
    //nrf_status = NRF_Status();
    while (NRF_Interrupt>0) 
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
        //printf("nrf_status DATA_READY\n");
        NRF_ReceivePayload(NRF_R_RX_PAYLOAD, 32, (uint8_t *)&packet);
        printf("nodeID: %d, X: %d, Y: %d, Z: %d\n", packet.nodeID, packet.accelX, packet.accelY, packet.accelZ);
        NRF_SendCommand(NRF_FLUSH_RX, 0xFF);
        REDval = RGB_PWM_TIMER_TOP + 25 - abs(packet.accelX / 4);
        GREENval = RGB_PWM_TIMER_TOP + 25 - abs(packet.accelY / 4);
        GPIO->P[RXEN_PORT].DOUT |= (1 << RXEN_PIN); 
        NRF_CE_hi;
      }
      
      INT_Disable();
      NRF_Interrupt--;
      INT_Enable();
      
    }
    if (MMA_Capture>0) {

  
      MMARegReadN(OUT_X_MSB_REG, 6, buf);
      accelReading.x = buf[0]<<8 | buf[1];
      accelReading.y = buf[2]<<8 | buf[3];
      accelReading.z = buf[4]<<8 | buf[5];
      REDval = RGB_PWM_TIMER_TOP + 25 - abs(accelReading.x / 16);
      GREENval = RGB_PWM_TIMER_TOP + 25 - abs(accelReading.y / 16);

      packet.type = PACKET_TYPE_ACCELDATA;
      packet.nodeID = NODE_ID;
      packet.accelX = accelReading.x;
      packet.accelY = accelReading.y;
      packet.accelZ = accelReading.z;

      //AccelReading.x = ((AccelReading.x << 8) & 0xFF00) | (AccelReading.x >> 8);
      //printf("X: %d, Y: %d, Z: %d\n", AccelReading.x, AccelReading.y, AccelReading.z);
      //NRF_PowerUp();
      NRF_TransmitPacket(32, (uint8_t *)&packet);
      //NRF_WriteRegister(NRF_CONFIG, 0x0E); // Power Up, Transmitter
      //NRF_SendCommand(0xE1, 0xFF);
      //NRF_CE_hi;
      RXEN_hi;
      //GPIO->P[RXEN_PORT].DOUT |= (1 << RXEN_PIN); 

    }    
    if((MMA_Capture<=0) && (NRF_Interrupt<=0)) 
    {
      //EMU_EnterEM1(); // send processor to sleep
    }
  }
}

