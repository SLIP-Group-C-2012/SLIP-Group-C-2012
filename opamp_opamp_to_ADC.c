/**************************************************************************//**
 * @file
 * @brief OPA1 connected to the ADC.
 * @author Energy Micro AS
 * @version 1.03
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
//#define OPAMP_PRESENT
//#define OPAMP_COUNT 1

#include "efm32.h"
#include "efm32_chip.h"
#include "efm32_opamp.h"
#include "efm32_cmu.h"
#include "efm32_adc.h"
#include <stdio.h>
#include "config.h"

static void ADCConfig(void)
{
    ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
    ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

    init.timebase = ADC_TimebaseCalc(0);
    init.prescale = ADC_PrescaleCalc(7000000, 0);
    ADC_Init(ADC0, &init);

    singleInit.reference = adcRefVDD;
    singleInit.input     = adcSingleInpCh1;
 
    ADC_InitSingle(ADC0, &singleInit);
}

int main(void)
{ 
    uint32_t voltage;

    CHIP_Init();
    SystemCoreClockUpdate();
    IO_Init();
    UART1->ROUTE = UART_ROUTE_LOCATION_LOC3
          | UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;
    uart_init(UART1); // for printf
    GPIO->P[0].DOUT &= ~(1 << 0);
    printf("started\n");
    
    
    /*Enable clocks*/
    CMU_ClockEnable(cmuClock_DAC0, true);
    CMU_ClockEnable(cmuClock_ADC0, true);

    /*Configure ADC*/
    ADCConfig();
              
    /*Define the configuration for OPA1*/
    OPAMP_Init_TypeDef configuration =  OPA_INIT_NON_INVERTING;
    
    /*Send the output to ADC*/
    configuration.outMode = opaOutModeAll;
    configuration.outPen =  DAC_OPA1MUX_OUTPEN_OUT4;

    /*Enable OPA1*/
    OPAMP_Enable(DAC0, OPA1, &configuration);

    /*While loop that get data from ADC and displays it on the LCD screen*/
    while (1)
    {
        /*Start ADC*/
        ADC_Start(ADC0, adcStartSingle);
        
        /*Wait until the ADC has warmed up*/
        while (ADC0->STATUS & ADC_STATUS_SINGLEACT);

        /*Get data from ADC and convert to voltage*/
        voltage = ADC_DataSingleGet(ADC0)*330/4096;
        
        /*Write to LCD*/
        printf("Voltage: %u\n", (unsigned int)voltage);
        
    } 
}
