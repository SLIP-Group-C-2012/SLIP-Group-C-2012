#include <stdio.h>
#include "audio_rec.h"
#include "dac.h"

#define record_time_in_s 3

int init(void)
{

}

int main(void)
{
  uint8_t cyclic_buf[SAMPLES_PER_SECOND * record_time_in_s] = {};
  
  record(cyclic_buf, SAMPLES_PER_SECOND * record_time_in_s, record_time_in_s);

  play((char *) cyclic_buf, SAMPLES_PER_SECOND * record_time_in_s);


  //printf("pcm_buf: ");
  //int i;
  //for (i = 0; i < SAMPLES_PER_SECOND * record_time_in_s; i++) {
  //  printf("%c", cyclic_buf[i]);
  //}
  //printf("\n");
  
  return 0;
}
