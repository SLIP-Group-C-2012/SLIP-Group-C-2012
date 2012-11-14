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
	//SystemCoreClockUpdate();

	printf("hello5");


	//CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
	//CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	//CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
	SystemCoreClockUpdate();
	//InitAudioPWM();

	printf("hello3");


	IO_Init();
	UART1->ROUTE = UART_ROUTE_LOCATION_LOC3
			| UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;

//	TIMER_IntClear(TIMER0, TIMER_IF_OF);
//	InitRGBLEDPWM();
//	TIMER_IntClear(TIMER0, TIMER_IF_OF);
//
//	TIMER_IntClear(TIMER2, TIMER_IF_OF);
//	InitTimoutTimer();
//	TIMER_IntClear(TIMER2, TIMER_IF_OF);

	uart_init(UART1); // for printf

	// for interrupts
	UART1->IEN = UART_IEN_RXDATAV;
	NVIC_EnableIRQ(UART1_RX_IRQn);

	//	uint32_t serial1;
	//	serial1 =  (*(uint32_t*)0x0FE081F0); //UNIQUE_0 is a timestamp (in Unix time) set in production,

	//	uint32_t serial2;
	//	serial2 =  (*(uint32_t*)0x0FE081F4); //UNIQUE_1 contains a facility ID

	printf("hello");
	InitAudioPWM();
	printf("hello2");

}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
//133761

#define SENDER (1)

#define COMPRESSED_SIZE (32)
#define AUDIO_PACK_SIZE (28)

#define SECONDS_TO_PLAY (1)
#define BUFFER_SIZE ((int) (8000*SECONDS_TO_PLAY))


int main(void)
{
	volatile unsigned long id = 0; // for counting loop
	uint8_t playback[BUFFER_SIZE];
	uint8_t data[32];

	init_config(); // init things for printf, interrupts, etc

	printf("I'm %s\n", SENDER ? "sender" : "receiver");

    uint8_t cyclic_buf[BUFFER_SIZE] = {};

	// turn on the radio on channel 2, with bandwidth 2MB and using maximum power
	radio_setup(2, BANDW_2MB, POW_MAX, 0);

    set_up_compression(AUDIO_PACK_SIZE, COMPRESSED_SIZE);

    while (1) {

#if SENDER
    record(cyclic_buf, BUFFER_SIZE, SECONDS_TO_PLAY);
    for (id = 0; id < BUFFER_SIZE - 32; id+=32)
        radio_sendPacket32((uint8_t *) &cyclic_buf[id]);
    printf("Finished sending\n");
#else
    if (radio_receivePacket32(data)) {

        if (id > sizeof(playback)-32) {
            play((char *) playback, sizeof(playback));
            id = 0;
            printf("Received\n");
        }
        memcpy(&playback[id], data, 28);
        id = id + 28;
    }
#endif

		// we should have this in our main loop always. It helps radio service itself.
		radio_loop();
	}
}

