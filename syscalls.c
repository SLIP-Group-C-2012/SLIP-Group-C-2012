#include <sys/stat.h>
#include "efm32.h"


#define UART_DR(baseaddr) (*(unsigned int *)(baseaddr))
#define UART_FR(baseaddr) (*(((unsigned int *)(baseaddr))+6))

#define USART_USED UART1

int _close(int file) {
  return 0;
}

int _fstat(int file, struct stat *st) {
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int file) {
  return 1;
}

int _lseek(int file, int ptr, int dir) {
  return 0;
}

int _open(const char *name, int flags, int mode) {
return -1;
}

int _read(int file, char *ptr, int len) {
  int todo;
  if(len == 0)
    return 0;
  while(!(USART_USED->STATUS & UART_STATUS_RXDATAV));
  *ptr++ = USART_USED->RXDATA;

  for(todo = 1; todo < len; todo++) {
    while(!(USART_USED->STATUS & UART_STATUS_RXDATAV));
    *ptr++ = USART_USED->RXDATA;
  }
  return todo;
}

static char *heap_end = 0;
caddr_t _sbrk(int incr) {
  extern char __cs3_heap_start; /* Defined by the linker */
  extern char __cs3_heap_end; /* Defined by the linker */
  char *prev_heap_end;
  if (heap_end == 0) {
    heap_end = &__cs3_heap_start;
  }
  prev_heap_end = heap_end;
  if (heap_end + incr > &__cs3_heap_end) {
    /* Heap and stack collision */
    return (caddr_t)0;
  }
  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

int _write(int file, char *ptr, int len) {
  int todo;
  for (todo = 0; todo < len; todo++) {
    while (!(USART_USED->STATUS & USART_STATUS_TXBL));
    USART_USED->TXDATA = *ptr++;
  }
  return len;
}