#include "efm32_int.h"

#define NUMOF_ADC_SAMPLES 100
#define SAMPLES_PER_SECOND 8000

int transfernumber = 0;

void record(uint32_t *pcm_buf, unsigned int pcm_bufsize, unsigned int numof_secs);

void start_recording(uint32_t *pcm_buf, unsigned int pcm_bufsize);
void stop_recording(void);

bool read_chunk(uint32_t **chunk);
