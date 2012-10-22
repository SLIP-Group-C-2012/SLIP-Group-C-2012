/*****************************************************************************
 * @file
 * @brief DMA Basic ADC transfer example
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
#include "dmactrl.h"

#define DMA_CHANNEL_ADC       0

/* DMA callback structure */
DMA_CB_TypeDef cb;

/* Transfer Flag */
volatile bool transferActive;

/* ADC Transfer Data */
#define ADCSAMPLES                        20
volatile uint16_t ramBufferAdcData[ADCSAMPLES];
#define ADCSAMPLESPERSEC              100000



/**************************************************************************//**
 * @brief  Call-back called when transfer is complete
 *****************************************************************************/
void transferComplete(unsigned int channel, bool primary, void *user)
{
  /* Stopping ADC by stopping TIMER0 */
   TIMER_Enable(TIMER0, false);
  
  printf("transfer complete!\n");
  
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
  CMU_ClockEnable(cmuClock_ADC0, true);  
  CMU_ClockEnable(cmuClock_TIMER0, true); 
  CMU_ClockEnable(cmuClock_PRS, true); 
}



/**************************************************************************//**
 * @brief Configure DMA for ADC RAM Transfer
 *****************************************************************************/
void setupDma(void)
{
  DMA_Init_TypeDef        dmaInit;
  DMA_CfgChannel_TypeDef  chnlCfg;
  DMA_CfgDescr_TypeDef    descrCfg;
  
  /* Initializing the DMA */
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;
  //dmaInit.controlBlock = 0;	// don't know what this does >.>
  
  DMA_Init(&dmaInit);
    
  /* Setting up call-back function */  
  cb.cbFunc  = transferComplete;
  cb.userPtr = NULL;

  /* Setting up channel */
  chnlCfg.highPri   = false;
  chnlCfg.enableInt = true;
  chnlCfg.select    = DMAREQ_ADC0_SINGLE;
  chnlCfg.cb        = &cb;
  DMA_CfgChannel(DMA_CHANNEL_ADC, &chnlCfg);

  /* Setting up channel descriptor */
  descrCfg.dstInc  = dmaDataInc2;
  descrCfg.srcInc  = dmaDataIncNone;
  descrCfg.size    = dmaDataSize2;
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(DMA_CHANNEL_ADC, true, &descrCfg);
    
  /* Setting flag to indicate that transfer is in progress
   * will be cleared by call-back function. */
  transferActive = true;
  
  /* Starting transfer. Using Basic since every transfer must be initiated
   * by the ADC. */
  DMA_ActivateBasic(DMA_CHANNEL_ADC,
                    true,
                    false,
                    (void *)&ramBufferAdcData,
                    (void *)&(ADC0->SINGLEDATA),
                    ADCSAMPLES - 1);
}



/**************************************************************************//**
 * @brief Configure TIMER to trigger ADC through PRS at a set sample rate
 *****************************************************************************/
void setupAdc(void)
{
  ADC_Init_TypeDef        adcInit       = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef  adcInitSingle = ADC_INITSINGLE_DEFAULT;
  
  /* Configure ADC single mode to sample Ref/2 */
  adcInit.prescale = ADC_PrescaleCalc(7000000, 0); /* Set highest allowed prescaler */
  ADC_Init(ADC0, &adcInit);
  
  adcInitSingle.input     =  adcSingleInpVrefDiv2;  /* Reference */
  adcInitSingle.prsEnable = true;                  
  adcInitSingle.prsSel    = adcPRSSELCh0;           /* Triggered by PRS CH0 */
  ADC_InitSingle(ADC0, &adcInitSingle);
  
  /* Connect PRS channel 0 to TIMER overflow */
  PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_TIMER0, PRS_CH_CTRL_SIGSEL_TIMER0OF, prsEdgeOff);
  
  /* Configure TIMER to trigger 100 kHz sampling rate */
  TIMER_TopSet(TIMER0,  CMU_ClockFreqGet(cmuClock_TIMER0)/ADCSAMPLESPERSEC);
  TIMER_Enable(TIMER0, true);
}



/**************************************************************************//**
 * @brief  Main function
 * This exmaple sets up the TIMER to trigger the ADC through PRS at a set
 * interval. The ADC then sets a DMA request and the DMA fetches each sample
 * until the a set number of samples have been received. 
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
  
  int i;
  printf("ramBufferAdcData: ");
  for (i = 0; i < ADCSAMPLES; i++)
    printf("%u ", (unsigned int) ramBufferAdcData[i]);
  printf("\n");
  
  /* Configuring clocks in the Clock Management Unit (CMU) */
  setupCmu();
  
  /* Configure DMA transfer from ADC to RAM */      
  setupDma();
  
  /* Configure ADC Sampling and TIMER trigger through PRS. Start TIMER as well */
  setupAdc();
  
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
  INT_Enable();
 
  printf("ramBufferAdcData: ");
  for (i = 0; i < ADCSAMPLES; i++)
    printf("%u ", (unsigned int) ramBufferAdcData[i]);
  printf("\n");
  
  printf("happy days!\n"); 
 
  /* Cleaning up after DMA transfers */
  DMA_Reset();

  /* Done */
  while (1);
}
