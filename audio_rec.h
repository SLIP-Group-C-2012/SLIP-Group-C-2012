#include "efm32_int.h"

#define NUMOF_ADC_SAMPLES 100
#define SAMPLES_PER_SECOND 8000

void record(uint8_t *pcm_buf, unsigned int pcm_bufsize, unsigned int numof_secs);

void start_recording(uint8_t *pcm_buf, unsigned int pcm_bufsize);
void stop_recording(void);

bool read_chunk(uint8_t **chunk);
