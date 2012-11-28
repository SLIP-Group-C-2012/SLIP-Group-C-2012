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
#include "dac.h"
#include "codec.h"

#include "config.h"
#include "radio.h"
#include "proto.h"

#include "acksys.h"

#include <math.h>
#include <string.h>

int pingtimer = 0;

void GPIO_EVEN_IRQHandler(void)
{
	radio_handleInterrupt();
}

void TIMER2_IRQHandler(void)// this is called every TIMER_RESEND_TOP* seconds //curently ~1
{
	//printf("Timer %d\n", pingtimer);

	TIMER_IntClear(TIMER2, TIMER_IF_OF);

	pingtimer++;

}

void TIMER0_IRQHandler(void)
{
	//printf("time");
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
	//InitAudioPWM();


	IO_Init();
	UART1->ROUTE = UART_ROUTE_LOCATION_LOC3
			| UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;

	uart_init(UART1); // for printf

	// for interrupts
	UART1->IEN = UART_IEN_RXDATAV;
	NVIC_EnableIRQ(UART1_RX_IRQn);

	InitAudioPWM();
	GPIO_PinModeSet(gpioPortD, 10, gpioModeInput, 0);

}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
//133761

#define COMPRESSED_SIZE (30)
#define AUDIO_PACK_SIZE (30)

#define SECONDS_TO_PLAY (1)
#define BUFFER_SIZE ((int) (8000*SECONDS_TO_PLAY))
#define RECEIVE_BUFF (8000)

void sendBuffer(uint8_t * buff, int size)
{
	int id2;
	for (id2 = 0; id2 < size - COMPRESSED_SIZE; id2+=COMPRESSED_SIZE) {
        proto_send((uint8_t *) &buff[id2], 0);
        radio_loop();
    }
}

int main(void)
{
	volatile unsigned long id = 0; // for counting loop
    unsigned int i = 0;
    bool need_audio_fix = false;
	uint8_t playback[RECEIVE_BUFF];
	uint8_t data[AUDIO_PACK_SIZE];
	uint8_t p[30];

	for (i = 0; i < 30; i++) p[i] = i;
	i = 0;

	init_config(); // init things for printf, interrupts, etc

    char input[30];

	// turn on the radio on channel 2, with bandwidth 2MB and using maximum power
	radio_setup(30, BANDW_2MB, POW_MAX, 1);

    //set_up_compression(AUDIO_PACK_SIZE, COMPRESSED_SIZE);

    uint8_t cyclic_buf[BUFFER_SIZE];
	uint8_t *chunk;
    start_recording(cyclic_buf, BUFFER_SIZE);

    while (1) {

        int available = read_chunk(&chunk);

        if (need_audio_fix) {
            i++;
            if (i >= 1000)
            {
                    char tmp = "";
                    proto_send((uint8_t *) &tmp, 2);
		        	radio_loop();
                    need_audio_fix = false;
            }
        }

		if( serial_getString( (uint8_t *)input) )
		{
			proto_send((uint8_t *) &input, 1);
			radio_loop();
	        need_audio_fix = true;
            i = 0;
		}

		if ( GPIO_PinInGet(gpioPortD, 10) == 0 ) {
	        need_audio_fix = true;
	        if (available) {
                proto_send(chunk, 0);
                radio_loop();
                i = 0;
            }
			//record(cyclic_buf, BUFFER_SIZE, SECONDS_TO_PLAY);


		} else {
            int ret = proto_receive(data);
			if (ret) {

				if (id > sizeof(playback) - AUDIO_PACK_SIZE) {

				    play(playback, sizeof(playback));
				    id = 0;
				}
				memcpy(&playback[id], data, AUDIO_PACK_SIZE);
				id = id + AUDIO_PACK_SIZE;
                if (ret == 2) {
                    need_audio_fix = true;
                    i = 0;
                }
			}
			radio_loop();
		}
	}
}
