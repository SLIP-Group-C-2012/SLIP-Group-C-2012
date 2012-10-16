/**************************************************************************//**
 * @file
 * @brief Simple LCD blink demo for EFM32_Gxxx_STK
 * @author Energy Micro AS
 * @version 2.1.2
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2009 Energy Micro AS, http://www.energymicro.com</b>
 ******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "efm32.h"
#include "efm32_chip.h"
#include "efm32_emu.h"
#include "efm32_gpio.h"
#include "efm32_i2c.h"
#include "efm32_usart.h"
#include "efm32_rtc.h"
#include "efm32_cmu.h"
#include "efm32_adc.h"
#include "efm32_timer.h"

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
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/

#define SENDER (0)

int main(void)
{
    char data[32];
    volatile int j;

    char ping[32] = "ping";
    char pong[32] = "pong";

    int i = 0;

    init_config();

    printf("I'm %s\n", SENDER ? "sender" : "receiver");

    radio_setup(2, BANDW_2MB, POW_MAX);

    while(1)
    {
        i++;

        if (i % 30000 == 0)
        {
            if (SENDER)
            {
                printf("Send: '%s'\n", ping);
                radio_sendPacket((uint8_t *) ping, 32);
            }
        }

        if (radio_receivePacket((uint8_t *) data, 32))
        {
            printf("Received: '%s'\n", data);

            if (strcmp(data, ping) == 0)
            {
                radio_sendPacket((uint8_t *) pong, 32);
                printf("Answer1: '%s'\n", pong);
            }
        }
    }
}

