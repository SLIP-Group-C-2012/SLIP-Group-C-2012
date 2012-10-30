#include "efm32_int.h"

//#define SAMPLES_PER_SECOND 8000
#define SAMPLES_PER_SECOND 1024

void record(uint8_t *pcm_buf, unsigned int pcm_bufsize, unsigned int numof_secs);
