#ifndef __DAC_H

void InitAudioPWM(void);

void play(uint8_t * data, int size);

void TIMER1_IRQHandler(void);

#endif

/*
    GPIO->P[4].DOUT |= (1 << 10);
    GPIO->P[4].MODEH = (GPIO->P[4].MODEH & ~_GPIO_P_MODEH_MODE10_MASK) | GPIO_P_MODEH_MODE10_PUSHPULL;
*/


/*
 CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
    SystemCoreClockUpdate();
    InitAudioPWM();
*/
