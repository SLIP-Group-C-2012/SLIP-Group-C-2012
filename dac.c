#include "efm32_cmu.h"
#include "efm32_timer.h"
#include "dac.h"

#define INPUT_BUF_SIZE (3*8000)

char buff[INPUT_BUF_SIZE];

int set=0;
int iterator = 0;
int toPlay = 0;


int next(int val)
{
    if(val == (INPUT_BUF_SIZE-1))
        val=0;
    else
        val++;
    return val;
}

void play(char * data, int size)
{
    for(int i = 0; i < size; i++ ) //
    {
        buff[(set = next(set))] = data[i];
        toPlay++;
    }
}


void InitAudioPWM(void)
{
  CMU_ClockEnable(cmuClock_TIMER1, true);
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
  TIMER_InitCC(TIMER1, 0, &timerCCInit);
  TIMER_InitCC(TIMER1, 1, &timerCCInit);
  //TIMER_InitCC(TIMER0, 2, &timerCCInit);

//  TIMER3->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_CC1PEN | TIMER_ROUTE_CC2PEN | TIMER_ROUTE_LOCATION_LOC0);

  TIMER1->ROUTE = TIMER_ROUTE_CC0PEN | TIMER_ROUTE_CC1PEN | TIMER_ROUTE_LOCATION_LOC1;
  /* Set Top Value */
  TIMER_TopSet(TIMER1, 255);//384

  /* Set compare value starting at top - it will be incremented in the interrupt handler */
  TIMER_CompareBufSet(TIMER1, 0, 256);//385
  TIMER_CompareBufSet(TIMER1, 1, 256);//385
  //TIMER_CompareBufSet(TIMER3, 2, RGB_PWM_TIMER_TOP + 1);

  /* Select timer parameters */
  TIMER_Init_TypeDef timerInit =
  {
    .enable     = true,
    .debugRun   = false,
    .prescale   = timerPrescale8,
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
  TIMER_IntEnable(TIMER1, TIMER_IF_OF);


  TIMER_IntClear(TIMER1, TIMER_IF_OF);
  /* Enable TIMER0 interrupt vector in NVIC */
  NVIC_EnableIRQ(TIMER1_IRQn);

  /* Configure timer */
  TIMER_Init(TIMER1, &timerInit);
}




void TIMER1_IRQHandler(void)
{
    int audio_Sample = 0;
    if(toPlay > 0)
    {
        audio_Sample = buff[(iterator = next(iterator))];
        toPlay--;
    }
    else
    {
        audio_Sample = 0;
    }
    TIMER_CompareBufSet(TIMER1, 0, (audio_Sample));

    TIMER_IntClear(TIMER1, TIMER_IF_OF);
}
