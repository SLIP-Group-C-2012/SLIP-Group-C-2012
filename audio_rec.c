#include <stdbool.h>
#include "efm32.h"
#include "efm32_dma.h"
#include "efm32_cmu.h"
#include "efm32_emu.h"
#include "efm32_adc.h"
#include "efm32_prs.h"
#include "efm32_gpio.h"
#include "efm32_opamp.h"
#include "efm32_timer.h"
#include "efm32_int.h"
#include "efm32_usart.h"
#include "dmactrl.h"
#include "dac.h"
#include "audio_rec.h"

#define DMA_CHANNEL_ADC 0

#define DMA_CHANNEL_TX   0
#define DMA_CHANNEL_RX   1
#define DMA_CHANNELS     2

typedef struct {
	uint32_t *pcm_buf;
	unsigned int pcm_bufsize;
} Dma;

Dma dma;

/* DMA callback structure */
DMA_CB_TypeDef cb;

/* Transfer Flag */
volatile bool transferActive;	// TODO: why is this volatile?
bool enable_transfer = false;

uint32_t *end_of_data;
uint32_t *read_pointer;	// a pointer to the chunk which should be read next

#define SAMPLE_RATE 8000  // 8000 hz sample rate

/**************************************************************************//**
 * @brief  ADC Interrupt handler
 *****************************************************************************/
void ADC0_IRQHandler(void)
{
	/* Clear interrupt flag */
	ADC_IntClear(ADC0, ADC_IFC_SINGLEOF);

	printf("ADC IRQ: DMA couldn't keep up with sample rate :(\n");

	while(1) {
		/* ERROR: ADC Result overflow has occured
		 * This indicates that the DMA is not able to keep up with the ADC sample
		 * rate and that a samples has been written to the ADC result registers
		 * before the DMA was able to fetch the previous result */
	}
}


int transfernumber = 0;
/**************************************************************************//**
 * @brief  Call-back called when transfer is complete
 *****************************************************************************/
void transferComplete(unsigned int channel, bool primary, void *user)
{
	printf("got dma interrupt!\n");

	Dma *dma = (Dma *) user;

	uint32_t *cyclic_buf = dma->pcm_buf;

	static int p = 2 * NUMOF_ADC_SAMPLES;

	/* Keeping track of the number of transfers */
	transfernumber++;


	if (p >= dma->pcm_bufsize)
		p = 0;  // move back to the beginning of the cycle

	/* Let the transfer be repeated a few times to illustrate re-activation */
	//if (dma->numof_pingpong_transfers == 0 || transfernumber < (dma->numof_pingpong_transfers)) {
	if (enable_transfer) {
		/* Re-activate the DMA */
		DMA_RefreshPingPong(channel,
		                    primary,
		                    false,
		                    &cyclic_buf[p],
		                    NULL,
		                    NUMOF_ADC_SAMPLES - 1,
		                    false);
	}
	else {
		/* Clearing Flag */
		transferActive = false;

		printf("transfer complete!\n");
	}

	p += NUMOF_ADC_SAMPLES;

	if (p >= dma->pcm_bufsize)
		end_of_data = cyclic_buf;
	else
		end_of_data = &cyclic_buf[p];

	if (read_pointer == end_of_data) {
		printf("warning: about to overwrite data that has not been read! :-(\n");
		enable_transfer = false;
	}
}



/**************************************************************************//**
 * @brief  Enabling clocks
 *****************************************************************************/
void setupCmu(void)
{
	CMU_ClockEnable(cmuClock_TIMER0, true);
	//CMU_ClockEnable(cmuClock_PRS, true);

	/* Enabling clocks */
	CMU_ClockEnable(cmuClock_DMA,  true);
	//CMU_ClockEnable(cmuClock_ADC0, true);

	// Try enabling DAC (experimental)
	CMU_ClockEnable(cmuClock_DAC0, true);
}

/* Introducing the Digital Microphone of Joy and Wonder! */
static void I2S_Setup(void)
{
	USART_InitI2s_TypeDef init = USART_INITI2S_DEFAULT;
	init.sync.autoTx = true;
	init.format = usartI2sFormatW32D32;

	CMU_ClockEnable(cmuClock_USART2, true);

	/* Use location 1: TX  - Pin D0, (RX - Pin D1) */
	/*                 CLK - Pin D2, CS - Pin D3   */


	GPIO_PinModeSet(gpioPortB, 3, gpioModePushPull, 0);	// tx
	GPIO_PinModeSet(gpioPortB, 4, gpioModeInput, 0);	// rx
	GPIO_PinModeSet(gpioPortB, 5, gpioModePushPull, 1);	// clock
	GPIO_PinModeSet(gpioPortB, 6, gpioModePushPull, 1);	// ws

	/*GPIO_PinModeSet(gpioPortE, 4, gpioModePushPull, 1);	// ws
	GPIO_PinModeSet(gpioPortE, 5, gpioModePushPull, 1);	// clock
	GPIO_PinModeSet(gpioPortE, 6, gpioModeInput, 0);	// rx
	GPIO_PinModeSet(gpioPortE, 7, gpioModePushPull, 0);	// tx*/
	
	// Turn the microphone on
	GPIO_PinModeSet(gpioPortF, 5, gpioModePushPull, 1);
	

	/* Configure USART for basic I2S operation */
	//init.sync.baudrate = wavHeader.frequency * 32;
	
	// what should the baud rate be?
	init.sync.baudrate = 512000;	// for 8k samples per sec
	
	USART_InitI2s(USART2, &init);

	USART2->TXDOUBLE = 0;	// start transmission

	/* Enable pins at location 1 */
	USART2->ROUTE = USART_ROUTE_RXPEN |
					USART_ROUTE_TXPEN |
	                USART_ROUTE_CSPEN |
	                USART_ROUTE_CLKPEN |
	                USART_ROUTE_LOCATION_LOC1;
}

/**************************************************************************//**
 * @brief
 *   Setup DMA in ping pong mode
 * @details
 *   The DMA is set up to transfer data from memory to the DAC, triggered by
 *   PRS (which in turn is triggered by the TIMER). When the DMA finishes,
 *   it will trigger the callback (PingPongTransferComplete).
 *****************************************************************************/
void setupDma(Dma *dma)
{
	/* DMA configuration structs */
	DMA_Init_TypeDef       dmaInit;
	DMA_CfgChannel_TypeDef chnlCfg;
	DMA_CfgDescr_TypeDef   descrCfg;

	/* Initializing the DMA */
	dmaInit.hprot        = 0;
	dmaInit.controlBlock = dmaControlBlock;
	
	DMA_Init(&dmaInit);

	/* Set the interrupt callback routine */
	cb.cbFunc = transferComplete;
	cb.userPtr = dma;

	/* channel 0 and 1 will need data at the same time,
	 * can use channel 0 as trigger */

	/* Setting up channel */
	chnlCfg.highPri   = false; /* Can't use with peripherals */
	chnlCfg.enableInt = true;  /* Interrupt needed when buffers are used */
	chnlCfg.select = DMAREQ_USART2_RXDATAV;	// receive from usart
	chnlCfg.cb = &cb;
	DMA_CfgChannel(DMA_CHANNEL_RX, &chnlCfg);
	

	/* Setting up channel descriptor */
	/* Source is USART register and doesn't move */
	descrCfg.srcInc = dmaDataIncNone;

	/* Transfer 32/16 bit each time to RAM */
	descrCfg.dstInc = dmaDataInc2;
	descrCfg.size   = dmaDataSize2;

	/* We have time to arbitrate again for each sample */
	descrCfg.arbRate = dmaArbitrate1;
	descrCfg.hprot   = 0;

	/* Configure both primary and secondary descriptor alike */
	DMA_CfgDescr(DMA_CHANNEL_RX, true, &descrCfg);
	DMA_CfgDescr(DMA_CHANNEL_RX, false, &descrCfg);

	uint32_t *cyclic_buf = dma->pcm_buf; // temporary work-around
	
	
	/* Enabling PingPong Transfer*/
	/*DMA_ActivatePingPong(0,
	                     false,
	                     (void *) & (USART1->TXDOUBLE),
	                     (void *) &ramBufferDacData0Stereo,
	                     (2 * BUFFERSIZE) - 1,
	                     (void *) &(USART1->TXDOUBLE),
	                     (void *) &ramBufferDacData1Stereo,
	                     (2 * BUFFERSIZE) - 1);*/

	printf("activate ping pong!\n");

	DMA_ActivatePingPong(0,
	                   false,
	                   (void *) cyclic_buf,
	                   (void *) &(USART2->RXDOUBLE),
	                   NUMOF_ADC_SAMPLES - 1,
	                   (void *) &cyclic_buf[NUMOF_ADC_SAMPLES],
	                   (void *) &(USART2->RXDOUBLE),
	                   NUMOF_ADC_SAMPLES - 1);
	                   
	printf("ping pong appears to be activated!\n");
}

/**************************************************************************//**
 * @brief Configure DMA for Ping-pong ADC to RAM Transfer
 *****************************************************************************/
/*void setupDma(Dma *dma)
{
	DMA_Init_TypeDef        dmaInit;
	DMA_CfgChannel_TypeDef  chnlCfg;
	DMA_CfgDescr_TypeDef    descrCfg;

	// Initializing the DMA
	dmaInit.hprot        = 0;
	dmaInit.controlBlock = dmaControlBlock;

	DMA_Init(&dmaInit);

	// Setup call-back function
	cb.cbFunc  = transferComplete;
	cb.userPtr = dma;

	// Setting up channel
	chnlCfg.highPri   = false;
	chnlCfg.enableInt = true;
	chnlCfg.select    = DMAREQ_ADC0_SINGLE;
	chnlCfg.cb        = &cb;
	DMA_CfgChannel(DMA_CHANNEL_ADC, &chnlCfg);

	// Setting up channel descriptor
	descrCfg.dstInc  = dmaDataInc1;	// tell dma to increment by a byte
	descrCfg.srcInc  = dmaDataIncNone;
	descrCfg.size    = dmaDataSize1;	// tell dma to read just one byte
	descrCfg.arbRate = dmaArbitrate1;
	descrCfg.hprot   = 0;


	DMA_CfgDescr(DMA_CHANNEL_ADC, true, &descrCfg); // configure primary descriptor
	DMA_CfgDescr(DMA_CHANNEL_ADC, false, &descrCfg);  // configure alternate descriptor

	// Setting flag to indicate that transfer is in progress
	//  will be cleared by call-back function
	transferActive = true;

	uint32_t *cyclic_buf = dma->pcm_buf; // temporary work-around

	// Enabling PingPong Transfer
	DMA_ActivatePingPong(DMA_CHANNEL_ADC,
	                     false,
	                     (void *) cyclic_buf,
	                     (void *)&(ADC0->SINGLEDATA),
	                     NUMOF_ADC_SAMPLES - 1,
	                     (void *)&cyclic_buf[NUMOF_ADC_SAMPLES],
	                     (void *)&(ADC0->SINGLEDATA),
	                     NUMOF_ADC_SAMPLES - 1);
}*/


/**************************************************************************//**
 * @brief Configure ADC to sample Vref/2 repeatedly at 10.5/13 Msamples/s
 *****************************************************************************/
void setupAdc(void)
{
	// Connect PRS channel 0 to TIMER overflow
	PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_TIMER0, PRS_CH_CTRL_SIGSEL_TIMER0OF, prsEdgeOff);

	//printf("CMU_ClockFreqGet(cmuClock_TIMER0): %d\n", CMU_ClockFreqGet(cmuClock_TIMER0));

	// Select TIMER0 parameters
	TIMER_Init_TypeDef timerInit =
	{
		.enable     = true,
		.debugRun   = true,
		.prescale   = timerPrescale1,
		.clkSel     = timerClkSelHFPerClk,
		.fallAction = timerInputActionNone,
		.riseAction = timerInputActionNone,
		.mode       = timerModeUp,
		.dmaClrAct  = false,
		.quadModeX4 = false,
		.oneShot    = false,
		.sync       = false,
	};

	/* For an 8000 Hz sample rate we need 8000 overflows per second
	 * with a 14Mhz clock we can get 8000 overflows per second by setting the
	 * 'top' value to 1750
	 *
	 * clock_frequency / sample rate = 1750
	 *
	 * Or...
	 *
	 * f = 1/t, f - frequency, t - time period
	 *
	 * t = 1 / 14 * 10^6
	 * (1 / 14 * 10^6) * 1750 = 1 / 8000 seconds
	 *
	 * So there will be 8000 overflows per second.
	 */
	TIMER_TopSet(TIMER0, 1750);
	TIMER_Enable(TIMER0, true);

	ADC_Init_TypeDef        adcInit       = ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef  adcInitSingle = ADC_INITSINGLE_DEFAULT;

	/* The ADC clock does not directly control the sample rate,
	 * the sample rate is controlled by the frequency of the PRS signal
	 * the PRS signal triggers and ADC conversion which then triggers a DMA transfer
	 */
	adcInit.prescale = ADC_PrescaleCalc(7000000, 0); // Set highest allowed prescaler (may as well)

	ADC_Init(ADC0, &adcInit);

	adcInitSingle.reference =  adcRefVDD;
	adcInitSingle.input = adcSingleInpCh1;

	//adcInitSingle.rep   = true; // do we need rep on?

	adcInitSingle.resolution = adcRes8Bit;
	adcInitSingle.prsEnable = true;
	adcInitSingle.prsSel = adcPRSSELCh0;

	ADC_InitSingle(ADC0, &adcInitSingle);

	/* Enable ADC single overflow interrupt to indicate lost samples */
	ADC_IntEnable(ADC0, ADC_IEN_SINGLEOF);
	NVIC_EnableIRQ(ADC0_IRQn);

	/* Start repetitive ADC single conversions */
	//ADC_Start(ADC0, adcStartSingle);
	//printf("adc setup [done]\n");
}

void setupOpAmp(void)
{
	OPAMP_Init_TypeDef configuration =  OPA_INIT_UNITY_GAIN;

	// Send the output to ADC
	configuration.outMode = opaOutModeAll;
	configuration.outPen =  DAC_OPA1MUX_OUTPEN_OUT4;

	OPAMP_Enable(DAC0, OPA1, &configuration);
}

/**************************************************************************//**
 * @brief  Main function
 * The ADC is set up to sample at full speed with a clock speed of half
 * the HFCORECLK used by the DMA. The DMA transfers the data to a RAM buffer
 * using ping-pong transfer.
 *****************************************************************************/
// TODO: rewrite this using start_recording and stop_recording....
void record(uint32_t *pcm_buf, unsigned int pcm_bufsize, unsigned int numof_secs)
{
	// the number of ping pong transfers that will occur in numof_secs time
	int transfer_limit = (SAMPLE_RATE / NUMOF_ADC_SAMPLES) * numof_secs;

	start_recording(pcm_buf, pcm_bufsize);

	printf("transfer_limit: %d\n", transfer_limit);

	while (transfernumber < transfer_limit)
		;

	stop_recording();
}

// TODO: test this actually works...
void start_recording(uint32_t *pcm_buf, unsigned int pcm_bufsize)
{
	enable_transfer = true;

	setupCmu();	// configure clocks in Clock Management Unit

	dma.pcm_buf = pcm_buf;
	dma.pcm_bufsize = pcm_bufsize;

	read_pointer = pcm_buf;
	end_of_data = pcm_buf;

	I2S_Setup();

	setupDma(&dma);	// configure dma to transfer from ADC to RAM using ping-pong
}

// TODO: test this actually works...
void stop_recording(void)
{
	enable_transfer = false;

	while (transferActive)
		;		// wait till transfer halted

	DMA_Reset();	// clean up after DMA transfers
}

// TODO: implement this...
bool read_chunk(uint32_t **chunk)
{
	if (read_pointer != end_of_data) {
		*chunk = read_pointer;
		read_pointer += NUMOF_ADC_SAMPLES;

		if (read_pointer >= dma.pcm_buf + dma.pcm_bufsize)
			read_pointer = dma.pcm_buf;  // move back to the beginning of the buffer

		return true;
	}
	else
		return false;
}
