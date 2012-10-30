#include <stdio.h>
#include "audio_rec.h"

#define record_time_in_s 10

int main(void)
{
  uint8_t cyclic_buf[SAMPLES_PER_SECOND * record_time_in_s] = {};
  
  record(cyclic_buf, SAMPLES_PER_SECOND * record_time_in_s, record_time_in_s);
  
  printf("pcm_buf: ");
  int i;
  for (i = 0; i < SAMPLES_PER_SECOND * record_time_in_s; i++) {
    printf("%d ", cyclic_buf[i]);
  }
  printf("\n");
  
  return 0;
}
