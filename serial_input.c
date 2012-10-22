#include "efm32_usart.h"
#include "serial_input.h"

#define INPUT_BUF_SIZE (32)

char buff[INPUT_BUF_SIZE];

int c=0;
int lock = 0;
int strend = 0;

void UART1_RX_IRQHandler(void) // INTERRUPT I NEED FOR UART 1
{
    char val = UART1->RXDATA;

    if (lock) return; // avoid corrupting data but drop this char

    if (c < INPUT_BUF_SIZE)
    {
        buff[c++] = val;

        strend = val == 0 || val == 10 || val == 13;
    }
}

int serial_receiveData(uint8_t * data)
{
    int data_to_copy = c;
    if (c == 0) return 0; // if nothing to copy

    lock = 1; // disable interrupts

    memcpy(data, buff, data_to_copy);
    c = 0; // reset counter
    strend = 0; // reset if waiting for string commands

    lock = 0; // enable interrupts

    return data_to_copy; // return number of bytes that have been copied
}

int serial_getString(uint8_t * str)
{
    if (c == 0)
        return 0;
    else if (c == INPUT_BUF_SIZE && !strend)
    {
        c = 0;
        return 0;
    }
    else if (strend) // if there is end of string inside
    {
        int i;

        lock = 1;

        memcpy(str, buff, c);

        // turn a new line into end of string
        for (i = 0; i < c; i++)
            if (str[i] == 13 || str[i] == 10 || str[i] == 0)
            {
                str[i] = 0;
                break;
            }

        c = 0;
        strend = 0;

        lock = 0;

        return 1;
    }
}
