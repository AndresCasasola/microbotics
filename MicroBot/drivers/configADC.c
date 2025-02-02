#include<stdint.h>
#include<stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "configADC.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


static QueueHandle_t cola_adc;



//Provoca el disparo de una conversion (hemos configurado el ADC con "disparo software" (Processor trigger)
void configADC_DisparaADC(void)
{
	ADCProcessorTrigger(ADC0_BASE,1);
}


void configADC_IniciaADC(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_ADC0);

    //HABILITAMOS EL GPIOE
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOE);
    // Enable pin PE3 for ADC AIN0|AIN1|AIN2|AIN3
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0);


    //CONFIGURAR SECUENCIADOR 1
    ADCSequenceDisable(ADC0_BASE,1);

    //Configuramos la velocidad de conversion al maximo (1MS/s)
    ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_RATE_FULL, 1);

    ADCSequenceConfigure(ADC0_BASE,1,ADC_TRIGGER_PROCESSOR,0);	//Disparo software (processor trigger)
    ADCSequenceStepConfigure(ADC0_BASE,1,0,ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE,1,1,ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE,1,2,ADC_CTL_CH2);
    ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_CH3|ADC_CTL_IE |ADC_CTL_END );	//La ultima muestra provoca la interrupcion
    ADCSequenceEnable(ADC0_BASE,1); //ACTIVO LA SECUENCIA

    //Habilita las interrupciones
    ADCIntClear(ADC0_BASE,1);
    ADCIntEnable(ADC0_BASE,1);
    IntPrioritySet(INT_ADC0SS1,configMAX_SYSCALL_INTERRUPT_PRIORITY);
    IntEnable(INT_ADC0SS1);

    // Hand ADC Sequencer 1 interrupt with configADC_ISR function
    // Next line if it is not specified in "startup_ccs.c"
    //IntRegister(INT_ADC0SS1, configADC_ISR);

    //Creamos una cola de mensajes para la comunicacion entre la ISR y la tarea que llame a configADC_LeeADC(...)
    cola_adc=xQueueCreate(8,sizeof(MuestrasADC));
    if (cola_adc==NULL)
    {
        while(1);
    }
}


void configADC_LeeADC(MuestrasADC *datos)
{
	xQueueReceive(cola_adc,datos,portMAX_DELAY);
}

void configADC_ResetQueue()
{
    xQueueReset(cola_adc);
}

void configADC_ISR(void)
{
	portBASE_TYPE higherPriorityTaskWoken=pdFALSE;

	MuestrasLeidasADC leidas;
	MuestrasADC finales;
	ADCIntClear(ADC0_BASE,1);//LIMPIAMOS EL FLAG DE INTERRUPCIONES
	ADCSequenceDataGet(ADC0_BASE,1,(uint32_t *)&leidas);//COGEMOS LOS DATOS GUARDADOS

	//Pasamos de 32 bits a 16 (el conversor es de 12 bits, asi que solo son significativos los bits del 0 al 11)
	finales.chan1=leidas.chan1;
	finales.chan2=leidas.chan2;
	finales.chan3=leidas.chan3;
	finales.chan4=leidas.chan4;

	//Guardamos en la cola
	xQueueSendFromISR(cola_adc,&finales,&higherPriorityTaskWoken);
	portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}

void configADC_ADCSet8BitResolution(void)
{
    ADCHardwareOversampleConfigure(ADC0_BASE, 0);   // 0 -> 8 Bits
}

void configADC_ADCSet12BitResolution(void)
{
    ADCHardwareOversampleConfigure(ADC0_BASE, 4);   // 4 -> 12 Bits
}

void configADC_SetTimerTrigger(void)
{
    ADCIntDisable(ADC0_BASE,ADC_INT_SS1);   // Disable Interrupts
    ADCSequenceDisable(ADC0_BASE,1);        // Disable Sequencer
    ADCSequenceConfigure(ADC0_BASE,1,ADC_TRIGGER_TIMER,0);  //Disparo hardware (timer trigger)
    ADCSequenceEnable(ADC0_BASE,1);         // Enable Interruption
    ADCIntEnable(ADC0_BASE,ADC_INT_SS1);    // Enable Sequencer
}

void configADC_SetProcessorTrigger(void)
{
    ADCIntDisable(ADC0_BASE,ADC_INT_SS1);   // Disable Interrupts
    ADCSequenceDisable(ADC0_BASE,1);        // Disable Sequencer
    ADCSequenceConfigure(ADC0_BASE,1,ADC_TRIGGER_PROCESSOR,0);  //Disparo software (processor trigger)
    ADCSequenceEnable(ADC0_BASE,1);         // Enable Interruption
    ADCIntEnable(ADC0_BASE,ADC_INT_SS1);    // Enable Sequencer
}

void configADC_TimerConfig(void)
{
    // Timer Configuration
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);           // Enable Timer Peripheral
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER5);      // SleepEnable Timer Peripheral
    TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);        // Configure periodic
    uint32_t ui32Period = (uint32_t)(SysCtlClockGet());     // Set system clock frequency
    TimerLoadSet(TIMER5_BASE, TIMER_A, ui32Period -1);      // Load freq
    TimerEnable(TIMER5_BASE, TIMER_A);                      // Enable Timer
}

void configADC_TimerStart(void)
{
    TimerControlTrigger(TIMER5_BASE, TIMER_A, true);        // Enable Timer Trigger
}

void configADC_TimerStop(void)
{
    TimerControlTrigger(TIMER5_BASE, TIMER_A, false);       // Disable Timer Trigger
}

void configADC_TimerSetFrequency(int f)
{
    TimerDisable(TIMER5_BASE, TIMER_A);             // Disable Timer
    ADCIntDisable(ADC0_BASE,ADC_INT_SS1);           // Disable ADC Interrupts
    uint32_t ui32Period = (uint32_t) ( (SysCtlClockGet() / (uint32_t) f) ); // Calcule period
    TimerLoadSet(TIMER5_BASE, TIMER_A, ui32Period -1);      // Load period
    TimerEnable(TIMER5_BASE, TIMER_A);              // Enable Timer
    ADCIntClear(ADC0_BASE, ADC_INT_SS1);            // Clear Interrupts flags
    ADCIntEnable(ADC0_BASE,ADC_INT_SS1);            // Enable ADC Interrupts
}
