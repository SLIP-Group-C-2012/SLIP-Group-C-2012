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
#include "NRF24.h"
#include "nRF24L01.h"
#include "nrf24_config.h"


void LETIMER_Setup(void)
{
}
void uart_init(USART_TypeDef *uart)
{
    static const uint32_t baud = 9600;
    static const uint32_t ovs = 16;
    static const uint32_t refclk = 14060000;

    uart->CLKDIV = (256 * refclk) / ((ovs * baud) - 1);
    uart->CMD = UART_CMD_TXEN | UART_CMD_RXEN;
    uart->IEN = UART_IEN_RXDATAV;
}

void InitTimoutTimer(void){
	  CMU_ClockEnable(cmuClock_TIMER1, true);

	  TIMER_Init_TypeDef timerInit =
	   {
	     .enable     = true,
	     .debugRun   = true,
	     .prescale   = timerPrescale1024,
	     //.prescale   = timerPrescale1024,
	     .clkSel     = timerClkSelHFPerClk,
	     .fallAction = timerInputActionNone,
	     .riseAction = timerInputActionNone,
	     .mode       = timerModeUp,
	     .dmaClrAct  = false,
	     .quadModeX4 = false,
	     .oneShot    = false,
	     .sync       = false,
	   };

	  /* Enable overflow interrupt */
	  TIMER_IntEnable(TIMER1, TIMER_IF_OF);

	  /* Enable TIMER0 interrupt vector in NVIC */
	  NVIC_EnableIRQ(TIMER1_IRQn);

	  /* Set TIMER Top value */
	  TIMER_TopSet(TIMER1, TIMER_RESEND_TOP);

	  /* Configure TIMER */
	  TIMER_Init(TIMER1, &timerInit);

}

void InitRGBLEDPWM(void)
{
  CMU_ClockEnable(cmuClock_TIMER0, true);
  /* Select CC channel parameters */
  TIMER_InitCC_TypeDef timerCCInit =
  {
    .eventCtrl  = timerEventEveryEdge,
    .edge       = timerEdgeBoth,
    .prsSel     = timerPRSSELCh0,
    .cufoa      = timerOutputActionNone,
    .cofoa      = timerOutputActionNone,
    .cmoa       = timerOutputActionToggle,
    .mode       = timerCCModePWM,
    .filter     = false,
    .prsInput   = false,
    .coist      = false,
    .outInvert  = false,
  };

    /* Configure CC channel 0 */
  TIMER_InitCC(TIMER0, 0, &timerCCInit);
  TIMER_InitCC(TIMER0, 1, &timerCCInit);
  //TIMER_InitCC(TIMER0, 2, &timerCCInit);

//  TIMER3->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_CC1PEN | TIMER_ROUTE_CC2PEN | TIMER_ROUTE_LOCATION_LOC0);

  TIMER0->ROUTE = TIMER_ROUTE_CC0PEN | TIMER_ROUTE_CC1PEN | TIMER_ROUTE_LOCATION_LOC0;
  /* Set Top Value */
  TIMER_TopSet(TIMER0, RGB_PWM_TIMER_TOP);

  /* Set compare value starting at top - it will be incremented in the interrupt handler */
  TIMER_CompareBufSet(TIMER0, 0, RGB_PWM_TIMER_TOP + 1);
  TIMER_CompareBufSet(TIMER0, 1, RGB_PWM_TIMER_TOP + 1);
  //TIMER_CompareBufSet(TIMER3, 2, RGB_PWM_TIMER_TOP + 1);

  /* Select timer parameters */
  TIMER_Init_TypeDef timerInit =
  {
    .enable     = true,
    .debugRun   = false,
    .prescale   = timerPrescale16,
    .clkSel     = timerClkSelHFPerClk,
    .fallAction = timerInputActionNone,
    .riseAction = timerInputActionNone,
    .mode       = timerModeUp,
    .dmaClrAct  = false,
    .quadModeX4 = false,
    .oneShot    = false,
    .sync       = false,
  };

  ///* Enable overflow interrupt */
  TIMER_IntEnable(TIMER0, TIMER_IF_OF);

  TIMER_IntClear(TIMER0, TIMER_IF_OF);
  /* Enable TIMER0 interrupt vector in NVIC */
  NVIC_EnableIRQ(TIMER0_IRQn);

  /* Configure timer */
  TIMER_Init(TIMER0, &timerInit);



}

void IO_Init(void)
{
    I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;
    RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;
    USART_InitSync_TypeDef usartInit = USART_INITSYNC_DEFAULT;
    RTC_Init_TypeDef rtcInits = RTC_INIT_DEFAULT;
    ADC_Init_TypeDef adcInit = ADC_INIT_DEFAULT;
    volatile uint32_t test = 0;

    /* Enable LE clock and LFXO oscillator */
    CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_LE;
    CMU->OSCENCMD |= CMU_OSCENCMD_LFXOEN;
    /* Wait until LFXO ready */
    /* Note that this could be done more energy friendly with an interrupt in EM1 */
    while (!(CMU->STATUS & CMU_STATUS_LFXORDY)) ;

    /* Select LFXO as clock source for LFACLK */
    CMU->LFCLKSEL = (CMU->LFCLKSEL & ~_CMU_LFCLKSEL_LFA_MASK) | CMU_LFCLKSEL_LFA_LFXO;

    CMU->OSCENCMD = CMU_OSCENCMD_LFRCOEN;

    /* Enable GPIO clock */
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;
      /* Initialize USART */
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_UART1;

    GPIO->P[4].DOUT |= (1 << 2);
    GPIO->P[4].MODEL =
              GPIO_P_MODEL_MODE2_PUSHPULL
            | GPIO_P_MODEL_MODE3_INPUT;




    GPIO->P[1].DOUT |= (1 << 3);
    /* Pin PB3 is configured to Push-pull */
    GPIO->P[1].MODEL = (GPIO->P[1].MODEL & ~_GPIO_P_MODEL_MODE3_MASK) | GPIO_P_MODEL_MODE3_PUSHPULL;
    /* Pin PB4 is configured to Input enabled */
    GPIO->P[1].MODEL = (GPIO->P[1].MODEL & ~_GPIO_P_MODEL_MODE4_MASK) | GPIO_P_MODEL_MODE4_INPUT;
    /* Pin PB5 is configured to Push-pull */
    GPIO->P[1].MODEL = (GPIO->P[1].MODEL & ~_GPIO_P_MODEL_MODE5_MASK) | GPIO_P_MODEL_MODE5_PUSHPULL;

    /* To avoid false start, configure output NRF_CSN as high on PB2 */
    GPIO->P[1].DOUT |= (1 << 2);
    /* Pin PB2 (NRF_CSN) is configured to Push-pull */
    GPIO->P[1].MODEL = (GPIO->P[1].MODEL & ~_GPIO_P_MODEL_MODE2_MASK) | GPIO_P_MODEL_MODE2_PUSHPULL;
    /* NRF_CE set to initial low */
    GPIO->P[1].DOUT &= ~(1 << 1);
    GPIO->P[1].MODEL = (GPIO->P[1].MODEL & ~_GPIO_P_MODEL_MODE1_MASK) | GPIO_P_MODEL_MODE1_PUSHPULL;
    // NRF_INT
    GPIO->P[1].MODEL = (GPIO->P[1].MODEL & ~_GPIO_P_MODEL_MODE0_MASK) | GPIO_P_MODEL_MODE0_INPUT;
    // RXEN
    GPIO->P[0].DOUT &= ~(1 << 2);
    GPIO->P[0].MODEL = (GPIO->P[0].MODEL & ~_GPIO_P_MODEL_MODE2_MASK) | GPIO_P_MODEL_MODE2_PUSHPULL;

    // US2_CS (MX25U64)
    GPIO->P[1].DOUT |= (1 << 6);
    GPIO->P[1].MODEL = (GPIO->P[1].MODEL & ~_GPIO_P_MODEL_MODE6_MASK) | GPIO_P_MODEL_MODE6_PUSHPULL;



    // I2C0 SCL
    // I2C0 SDA
    /* Enable clock for I2C0 */
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_I2C0;
    /* Enable signals SDA, SCL */
    I2C0->ROUTE |= I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN | I2C_ROUTE_LOCATION_LOC3;

    /* Enable clock for LETIMER0 */
    CMU->LFACLKEN0 |= CMU_LFACLKEN0_LETIMER0;

    /* Enable clock for RTC */
    CMU->LFACLKEN0 |= CMU_LFACLKEN0_RTC;


    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_USART2;
    usartInit.msbf = true;
    usartInit.clockMode = usartClockMode0;
    usartInit.baudrate = 7000000;
    USART_InitSync(USART2, &usartInit);
    //USART_Enable(USART1, usartEnable);
    USART2->ROUTE = (USART2->ROUTE & ~_USART_ROUTE_LOCATION_MASK) | USART_ROUTE_LOCATION_LOC1;
    /* Enable signals TX, RX, CLK */
    USART2->ROUTE |= USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN;

    GPIO_PinModeSet(gpioPortA, 0, gpioModeWiredAnd, 1);
    GPIO_PinModeSet(gpioPortA, 1, gpioModeWiredAnd, 1);
    GPIO_PinModeSet(gpioPortA, 3, gpioModeWiredAnd, 1);



    /* Use location 3: SDA - Pin D14, SCL - Pin D15 */
    /* Output value must be set to 1 to not drive lines low... We set */
    /* SCL first, to ensure it is high before changing SDA. */
    GPIO_PinModeSet(gpioPortD, 15, gpioModeWiredAnd, 1);
    GPIO_PinModeSet(gpioPortD, 14, gpioModeWiredAnd, 1);
    I2C_Init(I2C0, &i2cInit);
    I2C0->CLKDIV=1;

    // MMA_INT
    GPIO_PinModeSet(gpioPortA, 15, gpioModeInput, 0);
    GPIO_IntConfig(gpioPortA, 15, false, true, true);

    // NRF_INT
    GPIO_IntConfig(gpioPortB, 0, false, true, true);

    NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
    NVIC_EnableIRQ(GPIO_ODD_IRQn);
    CMU_ClockEnable(cmuClock_RTC, true);
    CMU_ClockEnable(cmuClock_ADC0, true);

  /* Each RTC tick is  second */

  CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_1);

  RTC_Reset();
  /* Enable RTC */
  //RTC_Enable(true);
  rtcInits.comp0Top = false;
  RTC_Init(&rtcInits);
   // RTC_Init(&rtcInit);
    //test = NRF_Status();
    //buf[0] = test;

    //test = MMARegRead(WHO_AM_I_REG);
}
