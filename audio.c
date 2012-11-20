#include <stdio.h>
#include "efm32_chip.h"
#include "audio_rec.h"
#include "dac.h"

#define record_time_in_s 1


int main(void)
{
  /* Initialize chip */
  CHIP_Init();
  SystemCoreClockUpdate();
  IO_Init();
  UART1->ROUTE = UART_ROUTE_LOCATION_LOC3
        | UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;
  uart_init(UART1); // for printf
  GPIO->P[0].DOUT &= ~(1 << 0);

  InitAudioPWM();

  uint8_t cyclic_buf[SAMPLES_PER_SECOND * record_time_in_s] = {};
  /*while (1) {
    record(cyclic_buf, SAMPLES_PER_SECOND * record_time_in_s, record_time_in_s);
    play((char *) cyclic_buf, SAMPLES_PER_SECOND * record_time_in_s);
  }*/
  
  start_recording(cyclic_buf, SAMPLES_PER_SECOND * record_time_in_s);
  
  uint8_t *chunk;  
  uint8_t buf[SAMPLES_PER_SECOND * record_time_in_s];
  uint8_t *s = buf;
  
  int t = 0;
  for (;;) {
  	/*if (read_chunk(&chunk)) {
  		memcpy(s, chunk, ADCSAMPLES * sizeof (uint8_t));
  		s += ADCSAMPLES;
	}*/
  	
  	/*if (s == buf + sizeof(buf)) {
  		//printf("play second %d\n", t++);
  		play(buf, SAMPLES_PER_SECOND * record_time_in_s);
  		
  		s = buf;
  	}*/
  	
  	if (read_chunk(&chunk))
  		play(chunk, ADCSAMPLES);
  }
  
  return 0;
}
