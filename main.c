#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "efm32.h"
#include "efm32_chip.h"
#include "efm32_emu.h"
#include "efm32_gpio.h"
#include "efm32_i2c.h"
#include "efm32_rtc.h"
#include "efm32_cmu.h"
#include "efm32_adc.h"
#include "efm32_timer.h"
#include "serial_input.h"

#include "config.h"
#include "radio.h"

#include <math.h>
#include <string.h>

void GPIO_EVEN_IRQHandler(void)
{
    radio_handleInterrupt();
}

void GPIO_ODD_IRQHandler(void)
{

}

void LETIMER0_IRQHandler(void)
{
}
void TIMER0_IRQHandler(void)
{
 // TIMER_CompareBufSet(TIMER0, 0, REDval);
// TIMER_CompareBufSet(TIMER0, 1, GREENval);
  TIMER_IntClear(TIMER0, TIMER_IF_OF);
}

int init_config(void)
{
    /* Chip errata */
    CHIP_Init();
    /* Ensure core frequency has been updated */
    SystemCoreClockUpdate();

    IO_Init();
    UART1->ROUTE = UART_ROUTE_LOCATION_LOC3
          | UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;

    TIMER_IntClear(TIMER0, TIMER_IF_OF);
    InitRGBLEDPWM();
    TIMER_IntClear(TIMER0, TIMER_IF_OF);
    uart_init(UART1); // for printf

    // for interrupts
    UART1->IEN = UART_IEN_RXDATAV;
    NVIC_EnableIRQ(UART1_RX_IRQn);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/

int main(void)
{
    char data[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // for sending
    char comp_in[32]; // for receiving from PC
    int v = 0; // value that I keep
    volatile unsigned long p_ti = 0; // for counting loop

    init_config(); // init things for printf, interrupts, etc

    printf("I'm %s\n", "transciever");

    // turn on the radio on channel 2, with bandwidth 2MB and using maximum power
    radio_setup(2, BANDW_2MB, POW_MAX);

    while(1)
    {
        p_ti++; // count loop iterations

        // send a packet occasionally
        if (p_ti % 500 == 0)
        {
            data[0] = v;
            //printf("%d. Send %d\n", p_ti, v);
            radio_sendPacket32((uint8_t *) data);
        }

        // demonstrate receiving of a packet
        if (radio_receivePacket32((uint8_t *) data))
        {
            printf("Received %d\n", data[0]);
            v = data[0]+1;
        }

        // demonstrate getting a command from the pc async
        if (serial_getString((uint8_t *) comp_in))
        {
            printf("Computer said '%s'\n", comp_in);
        }

        // we should have this in our main loop always. It helps radio service itself.
        radio_loop();
    }
}

