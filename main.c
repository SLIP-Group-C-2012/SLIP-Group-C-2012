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

#include "protcol.h"

#include <math.h>
#include <string.h>

int pingtimer = 0;

#define MY_ADDR (1)
#define DEST_ADDR (2)

typedef struct {
  uint8_t src;
  uint8_t dest;
  uint8_t packetID;
  uint8_t data[28];
} Packet_Type __attribute__ ((packed));

Packet_Type packet;

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

	TIMER_IntClear(TIMER0, TIMER_IF_OF);
	InitRGBLEDPWM();
	TIMER_IntClear(TIMER0, TIMER_IF_OF);

	TIMER_IntClear(TIMER2, TIMER_IF_OF);
	InitTimoutTimer();
	TIMER_IntClear(TIMER2, TIMER_IF_OF);

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

char array[] = {
    116,97,144,26,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,127,128,128,127,128,127,127,128,127,127,128,127,128,128,127,128,127,127,127,127,127,127,127,127,127,127,127,127,127,128,127,127,128,127,128,127,127,128,127,128,128,128,128,128,128,130,129,128,132,129,127,133,127,128,131,127,128,131,129,127,132,130,128,132,129,130,131,137,138,130,140,135,128,137,130,126,130,124,125,126,121,124,122,119,
    121,120,117,118,121,118,120,123,126,131,134,141,144,146,149,148,148,146,142,139,136,131,127,123,115,102,89,74,62,56,94,96,84,149,146,137,183,172,153,166,145,123,127,108,101,108,107,115,121,135,150,154,167,173,166,169,158,142,135,123,116,112,115,120,123,135,142,144,150,148,141,136,125,113,104,92,87,82,80,89,94,104,120,126,138,154,159,157,161,160,145,143,139,129,128,130,128,133,141,140,
    147,151,148,142,137,115,84,86,50,33,66,69,68,106,142,138,171,191,177,181,173,150,140,135,116,112,119,115,118,133,141,147,161,168,166,164,163,152,137,133,123,112,110,106,92,86,92,74,67,82,93,107,111,137,151,142,159,160,151,151,143,136,136,127,126,131,127,134,139,141,152,157,163,168,165,161,152,140,123,107,85,73,68,42,40,57,74,93,110,147,159,166,182,180,171,164,150,134,130,116,
    112,118,118,125,135,146,159,170,178,181,177,170,155,137,118,99,78,68,62,43,37,45,61,91,107,129,162,167,173,186,176,166,158,141,131,120,112,112,114,121,131,140,156,169,178,188,184,180,170,150,133,113,91,75,69,56,40,39,45,62,94,108,127,164,165,171,187,174,167,157,140,132,118,111,115,115,124,136,146,162,177,186,190,188,180,166,145,125,104,81,72,61,40,34,33,42,73,90,107,143,
    156,165,182,182,178,170,156,146,132,119,117,115,119,126,134,149,162,174,183,185,184,175,161,144,123,103,84,74,62,44,37,41,54,77,88,113,147,141,159,181,169,173,171,160,153,136,131,131,121,125,132,133,143,152,163,168,166,171,165,149,139,125,108,93,90,87,76,73,74,82,90,91,102,114,117,121,133,140,144,149,152,153,148,144,144,142,140,139,142,144,143,146,147,144,141,140,135,130,127,125,
    123,120,121,123,121,118,116,115,115,114,112,112,111,110,111,112,114,117,120,124,128,130,133,135,136,137,139,141,143,144,143,143,142,140,139,138,136,135,134,133,132,132,131,131,130,129,128,127,126,125,123,122,120,117,115,114,112,111,111,112,114,115,117,120,122,124,126,129,131,133,135,136,138,139,139,139,139,138,138,137,136,135,134,133,132,132,131,130,129,128,128,127,127,126,125,125,124,123,123,122,
    122,122,122,122,122,122,123,124,124,125,126,127,128,128,129,130,130,130,131,131,131,131,131,130,130,130,130,129,129,129,129,129,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,129,129,129,129,129,129,129,129,128,128,128,128,128,128,128,128,128,128,128,128,128,127,127,127,127,127,127,127,127,127,127,127,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,129,129,129,129,129,129,129,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,129,128,127,129,
    130,128,127,128,128,127,127,128,128,127,128,128,128,128,128,128,128,128,128,128,128,128,127,128,128,128,128,128,127,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,127,128,128,128,128,128,128,128,128,128,128,129,128,128,128,128,128,128,129,129,127,128,128,127,128,128,128,128,128,128,128,128,128,128,128,128,128,127,128,128,128,128,128,129,128,127,128,128,
    127,129,128,129,129,128,128,127,129,127,128,127,127,128,128,129,128,129,128,128,128,128,128,127,128,128,128,128,128,130,128,129,127,128,128,127,127,128,129,127,129,128,129,128,127,128,128,126,130,129,127,127,128,128,128,126,129,129,128,128,129,129,128,127,128,129,128,126,128,128,129,127,130,128,129,125,129,127,127,129,126,131,125,131,128,130,126,130,126,127,128,129,129,129,126,128,130,125,129,127,
    129,126,128,124,129,129,130,131,125,126,128,129,126,130,127,130,129,126,130,127,125,125,130,128,127,129,127,131,125,127,129,128,128,127,128,127,130,126,126,125,127,129,129,130,128,130,126,130,128,131,130,128,128,125,129,126,130,127,130,130,127,128,128,130,127,130,126,130,130,125,128,128,130,129,128,125,127,130,125,129,127,129,132,128,127,126,130,126,128,129,127,130,127,128,127,128,126,127,131,126,
    129,126,124,131,128,127,128,129,127,130,127,123,133,127,129,132,127,129,128,128,124,129,126,127,131,123,129,131,130,130,128,129,129,129,121,128,130,126,130,126,127,132,130,125,129,130,125,129,123,124,132,128,125,129,129,128,131,129,124,129,129,127,130,127,128,132,129,125,128,130,127,126,128,129,127,126,127,128,129,126,127,130,130,129,126,129,130,127,128,127,128,129,128,128,130,130,127,128,128,128,
    129,127,126,128,130,127,128,129,129,129,127,128,128,127,127,127,129,128,129,129,129,128,126,128,127,126,129,129,128,129,129,128,128,128,127,128,127,128,128,127,127,128,129,128,128,127,128,129,128,128,127,127,128,129,128,128,128,128,129,128,128,128,129,129,128,128,128,127,127,128,128,128,129,128,127,128,129,128,128,127,127,129,129,128,127,127,128,129,129,128,128,129,129,128,128,129,127,128,128,128,
    128,128,128,128,128,128,128,127,127,127,128,128,127,128,128,128,127,126,128,128,129,128,128,128,129,128,129,129,128,129,129,128,128,128,129,129,128,128,128,129,128,128,127,128,128,129,128,127,127,127,127,128,128,128,128,128,127,128,128,128,128,128,127,127,128,129,128,128,128,128,128,128,129,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    127,127,128,128,128,128,128,128,128,129,128,128,128,128,129,128,128,128,128,128,128,128,129,128,128,128,128,128,127,127,127,128,128,128,128,128,127,127,128,129,129,128,128,128,128,129,129,129,128,128,129,129,129,129,128,128,128,128,128,128,128,128,128,127,128,128,127,127,127,127,127,127,127,128,128,128,128,128,128,128,128,128,128,128,128,129,129,129,129,129,129,129,129,129,129,129,128,128,128,129,
    129,128,128,128,127,127,128,128,127,127,127,127,127,127,127,127,127,127,127,127,127,128,128,128,129,129,129,129,129,128,129,129,129,130,129,128,128,129,129,129,129,129,128,128,128,128,128,128,127,127,127,127,127,127,127,127,127,127,127,126,126,127,127,127,127,128,128,128,128,129,130,130,130,130,130,130,130,130,130,130,130,129,129,129,129,129,129,128,127,125,125,125,125,124,124,124,124,125,125,125,
    125,125,126,127,128,129,129,130,130,131,132,132,132,132,132,133,133,133,133,132,131,131,131,131,130,129,128,126,124,124,124,123,122,121,119,118,119,121,121,121,122,124,126,129,132,133,134,134,135,135,137,138,139,139,138,137,138,139,139,139,137,133,130,129,128,127,123,117,111,107,104,101,98,93,89,92,105,123,138,145,146,143,140,141,146,150,146,137,129,125,127,136,147,154,155,152,149,147,145,144,
    141,133,123,115,110,107,105,102,95,84,75,75,88,111,132,146,153,152,145,140,142,144,141,136,131,123,117,121,133,143,151,155,154,148,144,144,145,144,139,131,120,112,111,115,119,119,113,99,85,76,72,83,109,135,148,152,149,145,143,148,152,147,135,123,115,115,126,141,150,150,147,145,145,149,154,153,142,129,120,115,117,122,124,120,112,104,94,88,84,80,85,105,129,144,151,152,149,145,145,146,
    140,129,119,115,117,127,140,148,148,145,145,147,149,150,146,136,125,120,121,123,125,126,123,118,114,110,103,94,84,74,81,108,138,153,154,149,146,146,151,154,143,124,111,111,121,135,147,149,142,138,144,153,156,152,141,127,118,119,126,129,124,119,117,119,122,121,111,94,82,72,75,98,130,147,144,139,142,151,156,156,143,122,107,110,125,136,140,139,138,139,148,159,160,148,135,128,126,128,130,128,
    122,118,123,131,133,128,118,107,97,91,82,77,90,119,142,147,146,147,152,153,152,144,126,111,109,120,130,135,136,138,141,148,155,156,146,135,129,129,129,128,124,120,118,124,132,134,129,121,114,107,100,90,76,73,97,132,150,147,141,145,154,158,152,137,117,106,113,128,136,134,133,139,148,156,159,151,138,130,131,134,129,121,117,118,124,132,136,133,125,120,118,114,102,89,75,67,84,120,150,149,
    137,138,155,164,156,137,116,106,109,124,133,131,128,137,152,161,158,149,141,135,134,135,130,120,115,121,131,135,134,132,132,131,130,124,112,98,87,83,77,82,106,139,152,146,143,153,162,154,137,120,110,109,116,127,131,131,137,150,159,157,149,142,137,133,128,125,121,118,121,128,134,134,133,133,134,130,123,115,106,98,91,85,77,80,106,142,158,149,140,148,161,157,137,117,108,110,119,130,134,133,
    138,153,164,160,145,136,136,136,130,123,120,121,126,133,137,136,132,132,134,132,123,113,106,102,99,94,86,78,86,118,152,159,142,134,146,160,150,125,108,107,115,124,131,134,136,144,156,162,153,138,132,135,135,128,120,120,126,133,137,137,136,134,136,136,131,121,112,110,109,106,99,93,87,87,107,139,158,148,133,137,153,156,133,111,108,118,125,126,130,136,143,150,155,152,140,131,132,135,131,121,
    118,126,134,136,133,133,136,136,133,128,122,116,112,112,109,105,100,97,92,90,109,140,159,147,132,136,154,156,133,112,111,122,129,130,131,136,144,151,154,148,137,131,134,136,130,121,122,131,137,134,131,132,136,135,130,124,120,117,114,111,106,104,101,98,89,88,111,146,159,141,127,139,159,155,126,108,113,127,131,127,128,138,147,153,152,145,136,134,138,137,128,119,124,134,139,134,132,135,139,137,
    129,123,120,118,116,112,107,102,100,98,91,90,113,146,156,138,127,141,159,151,123,108,116,128,128,125,127,136,146,151,149,141,133,134,139,135,125,120,128,136,136,130,130,136,138,134,128,124,121,118,114,110,106,103,100,95,87,94,123,151,151,133,131,151,162,144,119,113,124,131,127,124,129,139,148,150,146,139,137,141,140,131,123,125,132,133,129,128,133,137,136,132,129,125,121,117,112,107,103,100,
    96,87,84,104,136,151,138,129,143,163,156,130,117,123,131,128,121,122,131,142,149,147,140,139,146,147,138,126,126,132,134,129,126,129,134,137,135,131,126,125,123,118,109,104,103,100,91,83,93,122,144,139,128,137,157,160,141,125,124,131,130,123,118,122,134,143,143,138,138,146,149,142,131,129,133,133,128,125,126,131,134,133,130,127,127,127,123,115,109,108,105,98,89,90,109,131,134,127,130,147,
    158,149,135,131,135,136,130,122,120,128,137,138,134,134,142,147,144,137,134,135,134,131,127,125,126,129,131,129,126,127,129,126,120,116,115,113,108,103,99,103,115,125,125,123,130,142,146,140,136,137,140,137,130,126,127,130,132,131,129,132,137,139,137,134,135,135,134,130,128,127,127,128,128,126,126,127,128,126,124,124,124,123,121,120,120,120,120,121,122,124,125,126,126,128,129,129,129,129,130,130,
    129,129,129,129,129,129,129,129,130,130,130,130,130,130,130,130,130,130,130,130,129,129,128,128,128,127,127,127,127,127,127,127,127,127,127,127,127,128,128,128,128,128,128,128,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,128,128,128,128,128,128,128,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,131,132,129,126,126,127,127,127,127,127,128,128,129,129,129,128,129,129,129,140,124,114,133,131,119,128,133,124,123,127,143,130,117,143,133,116,132,134,
    118,124,131,127,130,127,125,130,131,126,128,131,125,126,130,126,125,129,128,125,129,128,126,128,128,127,127,129,129,129,127,127,128,130,132,127,127,129,126,127,130,127,126,129,128,127,129,129,127,129,128,131,135,128,128,130,127,126,127,126,125,127,127,127,128,128,128,129,129,127,128,128,127,127,126,127,127,127,128,127,128,128,129,129,129,129,129,129,129,130,129,130,130,129,129,129,129,129,129,129,
    129,128,128,128,127,127,127,126,126,126,125,126,125,124,125,123,123,125,123,124,126,124,127,128,128,130,130,131,133,132,133,134,133,134,134,133,134,134,133,134,133,133,133,131,131,131,128,129,127,125,125,122,121,120,117,117,115,112,113,112,111,114,113,116,121,121,127,132,134,140,142,143,147,146,146,146,144,143,142,139,140,139,138,139,139,138,139,136,135,133,129,125,120,114,108,102,95,91,85,84,
    87,88,98,105,112,127,133,142,153,152,157,157,151,151,144,139,137,132,131,133,134,139,145,147,152,154,153,153,148,143,138,131,125,119,115,111,108,108,106,104,103,97,94,93,93,94,99,105,109,120,127,135,147,150,154,158,153,152,149,141,139,134,129,131,131,132,138,140,144,148,148,148,148,143,139,137,129,127,124,119,119,116,114,115,113,111,108,102,97,94,92,91,96,101,108,118,126,136,146,150,
    156,156,154,152,146,141,136,131,129,128,130,133,137,142,146,149,152,150,150,146,141,138,131,127,124,118,118,115,113,115,113,111,109,103,98,95,91,91,94,97,106,113,123,133,141,149,153,155,155,151,149,143,139,136,132,132,133,135,140,143,147,150,151,151,149,146,141,136,131,126,122,118,115,113,112,111,110,109,106,102,96,93,91,91,96,100,107,117,124,135,143,149,154,155,153,151,147,143,139,135,
    134,134,135,138,141,144,148,150,150,150,147,143,140,134,129,127,122,120,119,115,116,115,111,113,109,104,102,95,93,93,93,99,105,112,121,129,138,143,148,151,149,150,146,141,141,135,134,135,133,138,140,142,147,148,148,149,147,145,142,138,133,130,127,123,123,121,119,120,116,115,116,109,107,104,94,95,91,90,98,100,110,119,125,136,141,145,150,148,147,146,141,139,136,134,135,134,137,141,143,147,
    149,150,151,148,146,143,138,135,131,127,126,123,122,122,121,120,118,116,114,110,106,101,96,94,91,94,98,102,112,119,127,136,140,144,147,146,146,144,140,139,136,135,136,136,138,141,143,147,149,150,150,148,146,143,138,135,131,127,126,124,123,123,121,121,120,117,115,111,107,102,97,94,91,91,95,101,107,117,124,131,139,142,145,147,144,144,142,138,138,135,135,137,138,141,145,146,149,151,150,150,
    147,143,140,136,132,129,126,125,123,122,123,121,121,121,118,115,112,106,101,97,92,92,92,96,104,109,119,127,132,141,143,145,147,143,142,140,136,136,135,134,137,138,141,145,147,150,151,150,149,146,143,140,135,132,129,126,125,124,123,124,123,122,121,120,116,113,108,102,100,95,94,95,96,103,108,115,123,128,135,139,141,144,142,142,140,138,137,135,136,137,137,141,142,145,148,148,149,148,145,144,
    140,137,135,130,129,127,125,127,125,124,124,122,121,120,116,114,110,105,103,99,98,98,99,103,108,113,120,126,130,136,138,139,141,139,139,138,135,136,136,135,139,139,141,145,145,148,148,147,147,144,142,140,136,134,131,129,128,127,126,125,124,123,122,121,119,117,114,110,106,102,99,98,98,100,104,108,114,120,125,130,134,137,139,140,139,139,138,137,137,136,137,138,140,142,144,146,147,147,147,146,
    144,142,139,136,133,131,129,128,127,127,126,125,124,123,122,120,118,115,111,108,105,101,100,100,100,104,107,112,118,122,128,132,134,137,138,138,139,137,137,137,136,137,138,139,142,143,144,146,146,147,146,144,143,140,137,135,132,130,129,128,127,126,126,125,123,122,121,119,117,114,110,107,104,102,100,100,102,106,109,114,119,123,128,131,134,137,137,138,137,137,137,136,137,138,138,141,141,143,145,145,
    146,145,144,144,141,139,137,135,133,131,130,129,128,128,127,125,124,123,121,120,117,115,112,108,106,103,102,102,103,105,108,112,116,120,125,128,132,134,136,137,138,138,138,138,138,139,139,140,141,142,143,144,144,144,144,143,142,140,138,136,134,132,131,130,129,128,127,126,125,123,122,120,118,116,114,111,108,105,104,103,103,104,107,110,114,118,122,126,130,133,135,137,138,138,138,138,138,139,139,140,
    140,141,143,143,144,144,144,143,142,141,139,137,135,133,131,130,129,128,128,127,126,124,123,122,120,118,115,112,109,107,104,103,102,103,106,109,112,116,121,125,129,132,134,136,137,137,137,137,137,138,138,139,140,141,142,143,143,144,144,143,142,141,139,137,135,134,132,130,129,129,128,128,127,126,125,123,122,119,117,115,111,109,107,104,103,103,104,107,110,114,118,122,126,130,132,134,136,136,137,137,
    137,137,137,138,139,140,141,142,143,143,144,144,143,142,140,138,137,135,133,131,130,129,129,128,128,127,126,125,123,121,119,117,114,112,109,107,106,106,105,106,108,112,115,118,122,125,128,131,133,134,135,136,136,137,137,137,138,139,140,140,141,142,142,143,142,142,141,139,138,136,135,133,132,131,130,129,128,128,127,126,125,123,122,120,118,116,114,112,110,108,107,106,107,108,110,113,117,120,124,128,
    131,133,134,135,136,137,137,137,137,137,138,139,139,140,141,142,142,142,142,141,140,138,137,135,134,132,131,130,129,129,128,128,127,126,124,123,121,120,117,115,113,111,109,108,107,107,108,110,112,115,119,122,125,128,131,133,134,135,136,136,136,137,137,137,138,139,140,141,142,142,142,142,141,141,139,138,136,135,133,132,130,129,129,128,127,126,125,124,123,121,119,117,115,113,111,109,108,108,108,109,
    110,113,116,119,123,126,129,131,133,134,135,136,136,136,136,137,137,138,139,140,141,142,142,142,142,141,140,139,137,136,134,132,131,129,128,128,127,126,125,124,123,122,121,119,117,114,113,111,110,109,109,109,110,111,114,118,121,124,127,130,132,134,135,135,136,137,137,137,137,138,138,139,140,141,142,142,142,141,140,139,137,136,134,132,131,129,128,127,127,126,125,124,123,122,121,120,118,116,115,113,
    112,112,111,111,112,113,114,116,119,122,125,128,130,132,134,135,136,137,138,138,138,139,139,140,140,141,141,141,141,140,140,139,137,136,134,132,130,129,127,126,125,124,123,122,121,120,119,118,116,115,114,113,113,112,113,113,114,116,118,120,123,125,128,130,132,134,135,136,137,138,138,139,139,139,139,140,140,140,140,140,139,139,137,136,135,133,131,129,128,127,126,125,124,123,123,123,122,121,119,118,
    117,116,114,114,114,114,114,115,116,117,120,122,124,126,128,130,132,133,135,135,136,137,138,138,139,139,140,140,140,140,140,140,139,138,137,135,133,132,130,129,127,126,125,124,123,122,121,120,120,119,118,117,116,116,115,115,115,115,116,117,119,121,123,126,128,130,132,133,134,135,136,136,137,137,137,137,137,138,138,138,138,138,138,137,136,135,134,132,131,129,128,127,126,125,124,124,123,123,122,121,
    121,120,119,119,118,118,118,118,118,119,119,120,122,123,125,126,128,129,130,131,132,133,134,134,135,135,136,136,136,137,137,137,137,136,136,136,135,134,133,132,131,130,129,128,127,126,125,124,123,123,123,122,121,121,120,120,119,119,119,119,120,120,120,120,122,123,124,126,127,129,130,131,132,132,133,134,134,134,135,135,135,136,136,136,136,136,135,135,134,133,133,131,130,129,128,127,127,126,125,125,
    125,124,124,124,124,124,124,123,122,122,121,121,121,121,121,122,122,123,125,126,127,128,129,130,130,130,131,131,131,131,132,132,132,133,133,133,133,134,133,133,133,132,131,131,130,130,129,128,128,128,127,127,127,127,126,126,126,126,126,125,125,124,124,123,123,123,123,123,123,124,124,125,126,127,128,128,128,128,129,129,129,129,130,130,130,131,131,132,132,133,133,133,133,132,132,131,130,130,129,129,
    128,128,128,128,128,128,128,128,128,127,127,126,126,125,125,125,125,125,125,125,125,125,126,126,127,127,128,128,128,128,128,128,128,128,128,128,128,129,129,129,129,129,130,130,130,130,130,130,129,129,129,129,128,128,129,129,129,129,129,129,129,128,128,128,128,128,128,127,127,126,126,126,126,126,127,127,127,127,127,127,127,127,127,128,128,128,128,128,128,128,128,129,129,129,129,129,129,129,129,129,
    128,128,128,128,128,128,128,128,128,129,129,129,128,128,128,128,128,127,128,128,128,128,128,128,128,128,127,127,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,

};

#define SENDER

#define COMPRESSED_SIZE (32)
#define AUDIO_PACK_SIZE (28)
#define SECONDS_TO_PLAY (1)

int main(void)
{
	uint8_t audio_pack[COMPRESSED_SIZE];
	uint8_t uncompressed[AUDIO_PACK_SIZE];
	uint8_t non_realtime_buff[8000*SECONDS_TO_PLAY];
	volatile unsigned long p_ti = 0; // for counting loop

	init_config(); // init things for printf, interrupts, etc

	printf("I'm %s\n", "transciever");

	// turn on the radio on channel 2, with bandwidth 2MB and using maximum power
	radio_setup(10, BANDW_2MB, POW_MAX, 1);

    set_up_compression(AUDIO_PACK_SIZE, COMPRESSED_SIZE);
    
    int id = 0;
    int lastRet = 0;

	while(1)
	{
#ifdef SENDER
		if (p_ti > sizeof(array)-AUDIO_PACK_SIZE) p_ti = 0;
		//compress(&array[p_ti], audio_pack);
		id++;
		packet.src = MY_ADDR;
		packet.dest = DEST_ADDR;
		packet.packetID = id;
		packet.ack = 0;
		memcpy(packet.data, &array[p_ti], 28);
		radio_sendPacket32((uint8_t *)&packet);
		printf("SENT ::: Source : %d , dest : %d , ID : %d , data : %s\n", packet.src, packet.dest, packet.packetID, packet.data);
		p_ti += AUDIO_PACK_SIZE;
#else
        if(radio_receivePacket32((uint8_t *)&packet)) {
           
    		if(packet.dest == MY_ADDR)
    		{
    			if(packet.ack == 0)
    			{    			    			
    				printf("RECEIVED 4 ME ::: Source : %d , packetID : %d , data : %s\n", packet.src, packet.packetID, packet.data);
    				play((char *) &packet.data, 28);
    			} else if (packet.ack == 1)
    			{
    					
    			}    			
    		} else if(packet.packetID > lastRet){
    			printf("RETRANSMIT ::: Source : %d , packetID : %d , data : %s\n", packet.src, packet.packetID, packet.data);
    			lastRet = packet.packetID;    		
    			radio_sendPacket32((uint8_t *)&packet);
    		}

            memcpy(&non_realtime_buff[p_ti], audio_pack, sizeof(audio_pack));//uncompressed,sizeof(uncompressed));
            p_ti += sizeof(uncompressed);
        }
#endif

//		if (serial_getString((uint8_t *) comp_in))
//		{
//			// printf("clockspeed is'%i'\n", CMU_ClockFreqGet(cmuClock_TIMER0));
//			printf("input is '%s'\n Now Sending", comp_in);
//			// printf("String'%u'",strlen(str));
//			protocol_send(comp_in,0);
//
//		}

//		if(protocol_recive(packetbuff)){
//			printf("Data '%s'\n",packetbuff);
//		}
//
//		if (pingtimer>=10){
//			//printf("Timer %d\n", pingtimer);
//			protocol_send("PING",0);
//			pingtimer=0;
//			protocol_printalladdr();
//		}

		// we should have this in our main loop always. It helps radio service itself.
		//protocol_loop();
		radio_loop();
	}
}

