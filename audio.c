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

int main(void)
{
  uint8_t cyclic_buf[BUFSIZ * 10];
  
  int i;
  for (i = 0; i < BUFSIZ; i++)
  	cyclic_buf[i] = 0;
  
  start_recording(cyclic_buf);
  
  printf("scaled cyclic_buf: ");
  //for (i = 0; i < ADC_PINGPONG_TRANSFERS * ADCSAMPLES; i++) {
  for (i = 0; i < BUFSIZ * 10; i++) {
    printf("%d ", cyclic_buf[i]);
  }
  printf("\n");
  
  return 0;
}
