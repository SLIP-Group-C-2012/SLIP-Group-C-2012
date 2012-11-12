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
#include "efm32_gpio.h"
#include "efm32_system.h"
#include "efm32_timer.h"
#include "efm32_chip.h"


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
  GPIO_PinOutToggle(LED_PORT, LED_PIN);
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
  
  /* Configure pin as push pull output for LED/pin drive */
  GPIO_PinModeSet(LED_PORT, LED_PIN, gpioModePushPullDrive, 0);
  
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
  
  while(1)
  {
    /* Go to EM1 */
    EMU_EnterEM1();
  } 
 
}

