/*****************************************************************************
 * @file
 * @brief TIMER Demo Application
 * @author Energy Micro AS
 * @version 1.05
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2010 Energy Micro AS, http://www.energymicro.com</b>
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
#include "efm32.h"
#include "efm32_cmu.h"
#include "efm32_emu.h"
#include "efm32_i2c.h"
#include "efm32_gpio.h"
#include "efm32_system.h"
#include "efm32_timer.h"
#include "efm32_chip.h"
#include "i2cdrv.h"


/*#ifdef STK3700
#define LED_PORT gpioPortE
#define LED_PIN  2
#elif STK3300
#define LED_PORT gpioPortD
#define LED_PIN  7
#elif STKG8XX
#define LED_PORT gpioPortC
#define LED_PIN  0
#endif*/

#define LED_PORT gpioPortA
#define LED_PIN 0
#define GREEN_PIN 1

void transfer(uint8_t control, uint8_t data)
{
 /*I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = 0x7C;      // Parameter Address
  seq.flags = I2C_FLAG_WRITE_WRITE;     // Indicate combined write / read seq
  /* Select register to be read
  // Format: Reg ID, Value
  // Length must be specified
  seq.buf[0].data = &control;
  seq.buf[0].len = 1;
  seq.buf[1].data = &data;
  seq.buf[1].len = 1;*/

  /*do
  {
    ret = I2CDRV_Transfer(&seq);
    //GPIO_PinOutToggle(LED_PORT, GREEN_PIN);
  }   while (ret != i2cTransferDone);*/
  
  //if ((ret = I2CDRV_Transfer(&seq)) == i2cTransferDone)
    //GPIO_PinOutToggle(LED_PORT, GREEN_PIN);
    
  GPIO_PinOutToggle(LED_PORT, 1);
}

void turn_on(void)
{
  GPIO_PinOutToggle(LED_PORT, GREEN_PIN);
}

void init_screen(void)
{
  transfer(0x00,0x38);
  transfer(0x00,0x39);
  transfer(0x00,0x14);
  transfer(0x00,0x74);
  transfer(0x00,0x54);
  transfer(0x00,0x6F);
  transfer(0x00,0x0C);
  transfer(0x00,0x01);
}

/* 13761 Hz -> 14Mhz (clock frequency) / 1024 (prescaler)
  Setting TOP to 27342 results in an overflow each 2 seconds */
#define TOP 27342
/**************************************************************************//**
 * @brief TIMER0_IRQHandler
 * Interrupt Service Routine TIMER0 Interrupt Line
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{ 
  /* Clear flag for TIMER0 overflow interrupt */
  TIMER_IntClear(TIMER0, TIMER_IF_OF);
  
  /* Toggle LED ON/OFF */
  //GPIO_PinOutToggle(LED_PORT, LED_PIN);
  //GPIO_PinOutToggle(LED_PORT, 1);
}
/**************************************************************************//**
 * @brief  Main function
 * Main is called from __iar_program_start, see assembly startup file
 *****************************************************************************/
int main(void)
{  
  /* Initialize chip */
  CHIP_Init();
    
  /* Enable clock for GPIO module */
  CMU_ClockEnable(cmuClock_GPIO, true);
  
  /* Enable clock for TIMER0 module */
  CMU_ClockEnable(cmuClock_TIMER0, true);
  
  //CMU_ClockEnable(cmuClock_I2C0, true);
  
  /* Configure pin as push pull output for LED/pin drive */
  //GPIO_PinModeSet(LED_PORT, GREEN_PIN, gpioModePushPullDrive, 0);
  GPIO_PinModeSet(LED_PORT, GREEN_PIN, gpioModePushPull, 0);
  
  /* Select TIMER0 parameters */  
  TIMER_Init_TypeDef timerInit =
  {
    .enable     = true, 
    .debugRun   = true, 
    .prescale   = timerPrescale1024, 
    .clkSel     = timerClkSelHFPerClk, 
    .fallAction = timerInputActionNone, 
    .riseAction = timerInputActionNone, 
    .mode       = timerModeUp, 
    .dmaClrAct  = false,
    .quadModeX4 = false, 
    .oneShot    = false, 
    .sync       = false, 
  };
  
  /* Enable overflow interrupt */
  TIMER_IntEnable(TIMER0, TIMER_IF_OF);
  
  /* Enable TIMER0 interrupt vector in NVIC */
  NVIC_EnableIRQ(TIMER0_IRQn);
  
  /* Set TIMER Top value */
  TIMER_TopSet(TIMER0, TOP);
  
  /* Configure TIMER */
  TIMER_Init(TIMER0, &timerInit);
  
  // Starting screen stuff...
  GPIO_PinModeSet(gpioPortD,10,gpioModePushPull,0);
  GPIO_PinOutSet(gpioPortD,10);
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_I2C0;
  
  /* Enable signals SDA, SCL */
  I2C0->ROUTE |= I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN |
  I2C_ROUTE_LOCATION_LOC3;
  I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;
  I2C_Init(I2C0, &i2cInit);
  // Ending screen stuff...
  
  //init_screen();
  //transfer(0x40, '!');
  //turn_on();
  
  GPIO_PinOutToggle(LED_PORT, GREEN_PIN);
  
  while(1)
  {
    /* Go to EM1 */
    EMU_EnterEM1();
  } 
}

