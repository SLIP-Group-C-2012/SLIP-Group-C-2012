#include "efm32_int.h"

#define ADCSAMPLES 20	// TODO: ADCSAMPLES should be NUMOF_SAMPLES... probably
#define SAMPLES_PER_SECOND 8000
//#define SAMPLES_PER_SECOND 1024

void record(uint8_t *pcm_buf, unsigned int pcm_bufsize, unsigned int numof_secs);

void start_recording(uint8_t *pcm_buf, unsigned int pcm_bufsize, unsigned int numof_secs);
void stop_recording(void);

bool read_chunk(uint8_t **chunk);
