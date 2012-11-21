#include <stdio.h>
#include "efm32_chip.h"
#include "audio_rec.h"
#include "dac.h"

#define record_time_in_s 5

int main(void)
{
	/* Initialize chip */
	CHIP_Init();
	SystemCoreClockUpdate();
	IO_Init();
	UART1->ROUTE = UART_ROUTE_LOCATION_LOC3 | UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;
	uart_init(UART1); // for printf
	GPIO->P[0].DOUT &= ~(1 << 0);

	InitAudioPWM();

	uint8_t cyclic_buf[SAMPLES_PER_SECOND * record_time_in_s] = {};
	uint8_t *chunk;

	printf("o/\n");

	// Asynchronous
	//
	// Here be delays...
	/*start_recording(cyclic_buf, SAMPLES_PER_SECOND * record_time_in_s);

	for (;;) {
		printf("chunk: {");
		int i;
		for (i = 0; i < NUMOF_ADC_SAMPLES - 1; i++)
			printf("%d, ", chunk[i]);
		printf("%d}\n", chunk[NUMOF_ADC_SAMPLES - 1]);
	
		if (read_chunk(&chunk)) {
			//play(chunk, NUMOF_ADC_SAMPLES);
			;
		}
	}
	
	/*printf("Recording...\n");
	
	// Synchronous
	record(cyclic_buf, SAMPLES_PER_SECOND * record_time_in_s, record_time_in_s);
	
	printf("Playing...\n");
	
	play(cyclic_buf, SAMPLES_PER_SECOND * record_time_in_s);*/

	return 0;
}
