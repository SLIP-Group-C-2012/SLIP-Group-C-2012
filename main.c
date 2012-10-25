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

long timer0interuptcount = 0;

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

void TIMER1_IRQHandler(void)// this is called every 2 seconds
{
	printf("T1 Itterupt %s\n", "transciever");
	//timer0interuptcount = timer0interuptcount + 1;
	//printf("time");
	// TIMER_CompareBufSet(TIMER0, 0, REDval);
	// TIMER_CompareBufSet(TIMER0, 1, GREENval);
	TIMER_IntClear(TIMER1, TIMER_IF_OF);


//	timer0interuptcount = timer0interuptcount + 2;// add two as it only runs every 2 seconds
//	if (timer0interuptcount>5){
//		timer0interuptcount = 0; //reset at 30 seconds
//		//printf("Resend its been 30 seconds %s\n", "transciever");
//	}
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

	IO_Init();
	UART1->ROUTE = UART_ROUTE_LOCATION_LOC3
			| UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;

	TIMER_IntClear(TIMER0, TIMER_IF_OF);
	InitRGBLEDPWM();
	TIMER_IntClear(TIMER0, TIMER_IF_OF);

	TIMER_IntClear(TIMER1, TIMER_IF_OF);
	InitTimoutTimer();
	TIMER_IntClear(TIMER1, TIMER_IF_OF);

	uart_init(UART1); // for printf

	// for interrupts
	UART1->IEN = UART_IEN_RXDATAV;
	NVIC_EnableIRQ(UART1_RX_IRQn);


}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
//133761

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


	//char str[30] = "";// temporary data will contain keyboard strokes until a returnis detected it will then attempt to send
	//int endofinput = 0;
	//printf("%s\n", str);


	int lastsenttime = 0;
	int packetnumber = 0;
	int recivedupto = 0;
	//int sending = 0;

	while(1)
	{
		p_ti++; // count loop iterations

		if (serial_getString((uint8_t *) comp_in))
		{
			//strcat(str,comp_in);
			// printf("clockspeed is'%i'\n", CMU_ClockFreqGet(cmuClock_TIMER0));
			printf("input is '%s'\n Now Sending", comp_in);
			// printf("String'%u'",strlen(str));
			//sending = 1;
			send(comp_in);
		}


//		if(sending==1){
//			send(data)
//			if((lastsenttime+30)<timer0interuptcount){
//				//resend due to timeout
//				printf("timeout deincrement packet number");
//
//			}
//
//			if(recivedupto!=packetnumber){
//				//send next
//				printf("send pnum");
//				lastsenttime = (timer0interuptcount/300);
//				packetnumber++;
//			}
//		}

		/*
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
		 */
		// we should have this in our main loop always. It helps radio service itself.
		radio_loop();
	}
}

void send(char* textstring){

}


/*char[32] makepacket(char* textstring, char pnum)
{	
	char[2] header;
	header[0]=pnum;

        char[32] packet = strcat(header,textstring);
}
 */
