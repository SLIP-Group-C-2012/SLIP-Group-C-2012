#include <stdbool.h>
#include "efm32.h"
#include "efm32_chip.h"
#include "efm32_dma.h"
#include "efm32_cmu.h"
#include "efm32_emu.h"
#include "efm32_adc.h"
#include "efm32_prs.h"
#include "efm32_opamp.h"
#include "efm32_timer.h"
#include "efm32_int.h"
#include "dmactrl.h"

#define DMA_CHANNEL_ADC       0

/* DMA callback structure */
DMA_CB_TypeDef cb;

/* Transfer Flag */
volatile bool transferActive;

/* ADC Transfer Data */
#define ADC_PINGPONG_TRANSFERS            10

// TODO: ADCSAMPLES should be NUMOF_SAMPLES... probably
#define ADCSAMPLES                        20
volatile uint16_t ramBufferAdcData1[ADCSAMPLES];
volatile uint16_t ramBufferAdcData2[ADCSAMPLES];
#define ADCSAMPLESPERSEC              8000

int p = 0;

/**************************************************************************//**
 * @brief  ADC Interrupt handler
 *****************************************************************************/
void ADC0_IRQHandler(void)
{
  /* Clear interrupt flag */
  ADC_IntClear(ADC0, ADC_IFC_SINGLEOF);
  
  printf("ADC IRQ\n");
  
  while(1){
    /* ERROR: ADC Result overflow has occured
     * This indicates that the DMA is not able to keep up with the ADC sample
     * rate and that a samples has been written to the ADC result registers
     * before the DMA was able to fetch the previous result */ 
  }
}



/**************************************************************************//**
 * @brief  Call-back called when transfer is complete
 *****************************************************************************/
void transferComplete(unsigned int channel, bool primary, void *user)
{
  uint16_t *cyclic_buf = (uint16_t *) user;

  static int transfernumber = 0;
  uint16_t *data;
  if (primary) data = &ramBufferAdcData1;
  else data = &ramBufferAdcData2;
  
  // let's try transferring the contents of 'data' to a cyclic buffer
  int i;
  for (i = 0; i < ADCSAMPLES; i++)
    cyclic_buf[p + i] = data[i];
  
  //printf("transfer %d buffer %d [%d %d %d ... %d]\n", transfernumber, primary, data[0], data[1], data[2], data[ADCSAMPLES - 1]);
  
  /* Keeping track of the number of transfers */
  transfernumber++;
  
  if (transfernumber * ADCSAMPLES > BUFSIZ)
    p = 0;  // move back to the beginning of the cycle
    
  p += ADCSAMPLES;
  
  /* Let the transfer be repeated a few times to illustrate re-activation */
  if (transfernumber < (ADC_PINGPONG_TRANSFERS)) 
  {
    /* Re-activate the DMA */
    DMA_RefreshPingPong(channel,
                        primary,
                        false,
                        NULL,
                        NULL,
                        ADCSAMPLES - 1,
                        false);
  }
  
  else
  {
    /* Stopping ADC */
    ADC_Reset(ADC0);
       
    /* Clearing Flag */
    transferActive = false;
    
    printf("transfer complete!\n");
  } 
}



/**************************************************************************//**
 * @brief  Enabling clocks
 *****************************************************************************/
void setupCmu(void)
{
  CMU_ClockEnable(cmuClock_TIMER0, true); 
  CMU_ClockEnable(cmuClock_PRS, true); 
  
  /* Enabling clocks */
  CMU_ClockEnable(cmuClock_DMA,  true);  
  CMU_ClockEnable(cmuClock_ADC0, true);  
}



/**************************************************************************//**
 * @brief Configure DMA for Ping-pong ADC to RAM Transfer
 *****************************************************************************/
void setupDma(uint16_t *cyclic_buf)
{
  DMA_Init_TypeDef        dmaInit;
  DMA_CfgChannel_TypeDef  chnlCfg;
  DMA_CfgDescr_TypeDef    descrCfg;
  
  /* Initializing the DMA */
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;
  DMA_Init(&dmaInit);
  
  /* Setup call-back function */  
  cb.cbFunc  = transferComplete;
  cb.userPtr = cyclic_buf;  // put the cyclic buf here....

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
  DMA_CfgDescr(DMA_CHANNEL_ADC, false, &descrCfg);
  
  /* Setting flag to indicate that transfer is in progress
    will be cleared by call-back function */
  transferActive = true;

  /* Enabling PingPong Transfer*/  
  DMA_ActivatePingPong(DMA_CHANNEL_ADC,
                          false,
                          (void *)&ramBufferAdcData1,
                          (void *)&(ADC0->SINGLEDATA),
                          ADCSAMPLES - 1,
                          (void *)&ramBufferAdcData2,
                          (void *)&(ADC0->SINGLEDATA),
                          ADCSAMPLES - 1);
}


/**************************************************************************//**
 * @brief Configure ADC to sample Vref/2 repeatedly at 10.5/13 Msamples/s
 *****************************************************************************/
void setupAdc(void)
{
  ADC_Init_TypeDef        adcInit       = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef  adcInitSingle = ADC_INITSINGLE_DEFAULT;
  
  /* Configure ADC single mode to sample Vref/2 at 10.5 MHz */
  /* With a 21 MHz clock, this gives the DMA 26 cycles to fetch each result */
  adcInit.prescale = ADC_PrescaleCalc(8000, 0); /* Prescale to 10.5 MHz */
  ADC_Init(ADC0, &adcInit);
  
  adcInitSingle.input =  adcRefVDD; /* Reference */
  adcInitSingle.rep   = true;
  adcInitSingle.prsEnable = true;
  adcInitSingle.prsSel = adcPRSSELCh0; 
  ADC_InitSingle(ADC0, &adcInitSingle);
  
  /* Enable ADC single overflow interrupt to indicate lost samples */
  ADC_IntEnable(ADC0, ADC_IEN_SINGLEOF);
  NVIC_EnableIRQ(ADC0_IRQn);
  
  /* Start repetitive ADC single conversions */
  ADC_Start(ADC0, adcStartSingle);
  
  // -------------------------------------------------------------------------
  /* Connect PRS channel 0 to TIMER overflow */
  PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_TIMER0, PRS_CH_CTRL_SIGSEL_TIMER0OF, prsEdgeOff);

  /* Configure TIMER to trigger 100 kHz sampling rate */
  TIMER_TopSet(TIMER0,  CMU_ClockFreqGet(cmuClock_TIMER0)/ADCSAMPLESPERSEC);
  TIMER_Enable(TIMER0, true);
}

/**************************************************************************//**
 * @brief  Main function
 * The ADC is set up to sample at full speed with a clock speed of half
 * the HFCORECLK used by the DMA. The DMA transfers the data to a RAM buffer
 * using ping-pong transfer.
 *****************************************************************************/
void start_recording(uint16_t *cyclic_buf)
{ 
  /* Initialize chip */
  CHIP_Init();
  SystemCoreClockUpdate();
  IO_Init();
  UART1->ROUTE = UART_ROUTE_LOCATION_LOC3
        | UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;
  uart_init(UART1); // for printf
  GPIO->P[0].DOUT &= ~(1 << 0);
  printf("started recording...\n");
  
  /* Configuring clocks in the Clock Management Unit (CMU) */
  CMU_ClockEnable(cmuClock_TIMER0, true); 
  CMU_ClockEnable(cmuClock_PRS, true); 
  setupCmu();
  
  /* Configure DMA transfer from ADC to RAM using ping-pong */      
  setupDma(cyclic_buf);
  
  /* Configura ADC Sampling */
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
 
  /* Cleaning up after DMA transfers */
  DMA_Reset();
  
  printf("cyclic_buf: ");
  int i;
  for (i = 0; i < ADC_PINGPONG_TRANSFERS * ADCSAMPLES; i++) {
    printf("(%d, %d) ", i, cyclic_buf[i]);
  }
  printf("\n");
  
  printf("finished recording!\n");
}
