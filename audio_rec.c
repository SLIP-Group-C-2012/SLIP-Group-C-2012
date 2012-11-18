#include <stdbool.h>
#include "efm32.h"
#include "efm32_dma.h"
#include "efm32_cmu.h"
#include "efm32_emu.h"
#include "efm32_adc.h"
#include "efm32_prs.h"
#include "efm32_opamp.h"
#include "efm32_timer.h"
#include "efm32_int.h"
#include "dmactrl.h"
#include "dac.h"

#define DMA_CHANNEL_ADC 0

typedef struct {
	uint8_t *pcm_buf;
	unsigned int pcm_bufsize;
	unsigned int numof_pingpong_transfers;
} Dma;

/* DMA callback structure */
DMA_CB_TypeDef cb;

/* Transfer Flag */
volatile bool transferActive;	// TODO: why is this volatile?

#define SAMPLE_RATE 8000  // 8000 hz sample rate
#define ADCSAMPLES 20 // TODO: ADCSAMPLES should be NUMOF_SAMPLES... probably

/**************************************************************************//**
 * @brief  ADC Interrupt handler
 *****************************************************************************/
void ADC0_IRQHandler(void)
{
	/* Clear interrupt flag */
	ADC_IntClear(ADC0, ADC_IFC_SINGLEOF);

	printf("ADC IRQ: DMA couldn't keep up with ADC sample rate :(\n");

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
	Dma *dma = (Dma *) user;

	uint8_t *cyclic_buf = dma->pcm_buf;

	static int p = 2 * ADCSAMPLES;

	//play((char *) &cyclic_buf[transfernumber * ADCSAMPLES], ADCSAMPLES);

	/* Keeping track of the number of transfers */
	transfernumber++;


	if (p >= dma->pcm_bufsize)
		p = 0;  // move back to the beginning of the cycle

	/* Let the transfer be repeated a few times to illustrate re-activation */
	if (dma->numof_pingpong_transfers == 0 || transfernumber < (dma->numof_pingpong_transfers)) {
		/* Re-activate the DMA */
		DMA_RefreshPingPong(channel,
		                    primary,
		                    false,
		                    &cyclic_buf[p],
		                    NULL,
		                    ADCSAMPLES - 1,
		                    false);
	}
	else {
		/* Stopping ADC */
		ADC_Reset(ADC0);

		/* Clearing Flag */
		transferActive = false;

		//printf("transfer complete!\n");
	}

	p += ADCSAMPLES;
}



/**************************************************************************//**
 * @brief  Enabling clocks
 *****************************************************************************/
void setupCmu(void)
{
	CMU_ClockEnable(cmuClock_TIMER0, true);
	CMU_ClockEnable(cmuClock_PRS, true);

	/* Enabling clocks */
	CMU_ClockEnable(cmuClock_DMA,  true);
	CMU_ClockEnable(cmuClock_ADC0, true);

	// Try enabling DAC (experimental)
	CMU_ClockEnable(cmuClock_DAC0, true);
}



/**************************************************************************//**
 * @brief Configure DMA for Ping-pong ADC to RAM Transfer
 *****************************************************************************/
void setupDma(Dma *dma)
{
	DMA_Init_TypeDef        dmaInit;
	DMA_CfgChannel_TypeDef  chnlCfg;
	DMA_CfgDescr_TypeDef    descrCfg;

	/* Initializing the DMA */
	dmaInit.hprot        = 0;
	dmaInit.controlBlock = dmaControlBlock;

	DMA_Init(&dmaInit);

	/* Setup call-back function */
	cb.cbFunc  = transferComplete;
	cb.userPtr = dma;

	/* Setting up channel */
	chnlCfg.highPri   = false;
	chnlCfg.enableInt = true;
	chnlCfg.select    = DMAREQ_ADC0_SINGLE;
	chnlCfg.cb        = &cb;
	DMA_CfgChannel(DMA_CHANNEL_ADC, &chnlCfg);

	/* Setting up channel descriptor */
	descrCfg.dstInc  = dmaDataInc1;	// tell dma to increment by a byte
	descrCfg.srcInc  = dmaDataIncNone;
	descrCfg.size    = dmaDataSize1;	// tell dma to read just one byte
	descrCfg.arbRate = dmaArbitrate1;
	descrCfg.hprot   = 0;


	DMA_CfgDescr(DMA_CHANNEL_ADC, true, &descrCfg); // configure primary descriptor
	DMA_CfgDescr(DMA_CHANNEL_ADC, false, &descrCfg);  // configure alternate descriptor

	/* Setting flag to indicate that transfer is in progress
	  will be cleared by call-back function */
	transferActive = true;

	uint8_t *cyclic_buf = dma->pcm_buf; // temporary work-around

	/* Enabling PingPong Transfer*/
	DMA_ActivatePingPong(DMA_CHANNEL_ADC,
	                     false,
	                     (void *) cyclic_buf,
	                     (void *)&(ADC0->SINGLEDATA),
	                     ADCSAMPLES - 1,
	                     (void *)&cyclic_buf[ADCSAMPLES],
	                     (void *)&(ADC0->SINGLEDATA),
	                     ADCSAMPLES - 1);
}


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
void record(uint8_t *pcm_buf, unsigned int pcm_bufsize, unsigned int numof_secs)
{
	transfernumber = 0;
	//printf("started recording...\n");

	setupCmu();	// configure clocks in Clock Management Unit

	Dma dma;
	dma.pcm_buf = pcm_buf;
	dma.pcm_bufsize = pcm_bufsize;
	dma.numof_pingpong_transfers = (SAMPLE_RATE / ADCSAMPLES) * numof_secs;

	//printf("BUFSIZ: %d\n", BUFSIZ);
	//printf("dma.pcm_bufsize: %d\n", dma.pcm_bufsize);
	//printf("dma.numof_pingpong_transfers: %d\n", dma.numof_pingpong_transfers);

	setupDma(&dma);	// configure dma to transfer from ADC to RAM using ping-pong

	setupOpAmp();

	setupAdc();
	/* Wait in EM1 in until DMA is finished and callback is called */
	/* Disable interrupts until flag is checked in case DMA finishes after flag
	* check but before sleep command. Device will still wake up on any set IRQ
	* and any pending interrupts will be handled after interrupts are enabled
	* again. */
	INT_Disable();
	while(transferActive)
	{
		EMU_EnterEM1();
		INT_Enable();
		INT_Disable();
	}
	INT_Enable();

	DMA_Reset();	// clean up after DMA transfers

	//printf("finished recording!\n");
}

// TODO: test this actually works...
void start_recording(uint8_t *pcm_buf, unsigned int pcm_bufsize, unsigned int numof_secs)
{
	enable_transfer = true;

	setupCmu();	// configure clocks in Clock Management Unit

	Dma dma;
	dma.pcm_buf = pcm_buf;
	dma.pcm_bufsize = pcm_bufsize;
	dma.numof_pingpong_transfers = (SAMPLE_RATE / ADCSAMPLES) * numof_secs;

	//printf("BUFSIZ: %d\n", BUFSIZ);
	//printf("dma.pcm_bufsize: %d\n", dma.pcm_bufsize);
	//printf("dma.numof_pingpong_transfers: %d\n", dma.numof_pingpong_transfers);

	setupDma(&dma);	// configure dma to transfer from ADC to RAM using ping-pong

	setupOpAmp();

	setupAdc();
}

// TODO: test this actually works...
void stop_recording(void)
{
	enable_transfer = false
	
	while (transferActive)
		;		// wait till transfer halted

	DMA_Reset();	// clean up after DMA transfers
}

