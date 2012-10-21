/*****************************************************************************
 * @file
 * @brief DMA Flash to RAM transfer example
 * @author Energy Micro AS
 * @version 2.01
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2012 Energy Micro AS, http://www.energymicro.com</b>
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
#include <stdbool.h>
#include "efm32.h"
#include "efm32_chip.h"
#include "efm32_dma.h"
#include "efm32_cmu.h"
#include "efm32_emu.h"
#include "efm32_adc.h"
#include "efm32_prs.h"
#include "efm32_timer.h"
#include "efm32_int.h"

// From the abyss;
/* DMA control block, must be aligned to 256. */
#if defined (__ICCARM__)
#pragma data_alignment=256
DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2];
#elif defined (__CC_ARM)
DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2] __attribute__
((aligned(256)));
#elif defined (__GNUC__)
DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2] __attribute__
((aligned(256)));
#else
#error Undefined toolkit, need to define alignment
#endif 

/*#include "efm32.h"
#include "efm32_chip.h"
#include "efm32_dma.h"
#include "efm32_cmu.h"
#include "efm32_emu.h"
#include "efm32_int.h"*/
//#include "dmactrl.h"

#define DMA_CHANNEL_FLASHTORAM       0

/* DMA callback structure */
DMA_CB_TypeDef cb;

/* Transfer Flag */
volatile bool transferActive;

/* Flash Transfer Data */
const uint32_t flashData[] = {0,1,2,3,4,5,6,7,8,9};
#define FLASHDATA_SIZE (sizeof(flashData)/sizeof(uint32_t))
volatile uint32_t ramBufferFlashData[FLASHDATA_SIZE];



/**************************************************************************//**
 * @brief  Call-back called when flash transfer is complete
 *****************************************************************************/
void transferComplete(unsigned int channel, bool primary, void *user)
{
  /* Clearing flag to indicate that transfer is complete */
  transferActive = false;  
}



/**************************************************************************//**
 * @brief  Enable clocks
 *****************************************************************************/
void setupCmu(void)
{
  /* Enable clocks */
  CMU_ClockEnable(cmuClock_DMA, true);  
}



/**************************************************************************//**
 * @brief Configure DMA for Flash to RAM transfer
 *****************************************************************************/
void setupDma(void)
{
  DMA_Init_TypeDef        dmaInit;
  DMA_CfgChannel_TypeDef  chnlCfg;
  DMA_CfgDescr_TypeDef    descrCfg;
  
  /* Initializing the DMA */
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;
  //dmaInit.controlBlock = 0; // total hack
  
  DMA_Init(&dmaInit);

  /* Setting call-back function */  
  cb.cbFunc  = transferComplete;
  cb.userPtr = NULL;

  /* Setting up channel */
  chnlCfg.highPri   = false;
  chnlCfg.enableInt = true;
  chnlCfg.select    = 0;
  chnlCfg.cb        = &(cb);
  DMA_CfgChannel(DMA_CHANNEL_FLASHTORAM, &chnlCfg);

  /* Setting up channel descriptor */
  descrCfg.dstInc  = dmaDataInc4;
  descrCfg.srcInc  = dmaDataInc4;
  descrCfg.size    = dmaDataSize4;
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(DMA_CHANNEL_FLASHTORAM, true, &descrCfg);
}

/**************************************************************************//**
 * @brief  Main function
 * This example shows how to copy a block of data from the flash to RAM using
 * a DMA auto transfer.
 *****************************************************************************/
int main(void)
{ 
  /* Initialize chip */
  CHIP_Init();
  SystemCoreClockUpdate();
  IO_Init();
  UART1->ROUTE = UART_ROUTE_LOCATION_LOC3 | UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;
  uart_init(UART1); // for printf
  GPIO->P[0].DOUT &= ~(1 << 0);

  printf("FLASHDATA_SIZE: %d\n", FLASHDATA_SIZE);
  

  printf("ramBufferFlashData: ");
  int i;
  for (i = 0; i < FLASHDATA_SIZE; i++)
    printf("%d ", ramBufferFlashData);
  printf("\n");
  
  /* Configuring clocks in the Clock Management Unit (CMU) */
  setupCmu();
  
  /* Configure DMA transfer from Flash to RAM */      
  setupDma();
  
  /* Setting flag to indicate that transfer is in progress
  *  will be cleared by call-back function */
  transferActive = true;
  
  /* Starting the transfer. Using Auto (all data is transfered at once) */
  DMA_ActivateAuto(DMA_CHANNEL_FLASHTORAM,
                       true,
                       (void *)&ramBufferFlashData,
                       (void *)&flashData,
                       FLASHDATA_SIZE - 1);
  
  
  /* Wait in EM1 in until DMA is finished and callback is called */
  /* Disable interrupts until flag is checked in case DMA finishes after flag 
  * check but before sleep command. Device will still wake up on any set IRQ 
  * and any pending interrupts will be handled after interrupts are enabled 
  * again. */
  INT_Disable();
  while(transferActive)
  {
   EMU_EnterEM1(); 
   INT_Enable();
   INT_Disable();
  }
  
  printf("happy days!\n");
  
  printf("ramBufferFlashData: ");
  for (i = 0; i < FLASHDATA_SIZE; i++)
    printf("%d ", ramBufferFlashData);
  printf("\n");
  
  INT_Enable();
 
  /* Cleaning up after DMA transfers */
  DMA_Reset();

  /* Done */
  while (1);
}
