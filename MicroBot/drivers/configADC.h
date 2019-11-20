/*
 * configADC.h
 *
 *  Created on: 22/4/2016
 *      Author: jcgar
 *  Upgraded on: 18/11/2019
 *      Author: Andres Casasola Dominguez
 */

#ifndef CONFIGADC_H_
#define CONFIGADC_H_

#include <stdint.h>

typedef struct
{
	uint16_t chan1;
	uint16_t chan2;
	uint16_t chan3;
	uint16_t chan4;
} MuestrasADC;

typedef struct
{
	uint32_t chan1;
	uint32_t chan2;
	uint32_t chan3;
	uint32_t chan4;
} MuestrasLeidasADC;


void configADC_ISR(void);
void configADC_DisparaADC(void);
void configADC_LeeADC(MuestrasADC *datos);
void configADC_IniciaADC(void);
void configADC_ADCSet8BitResolution(void);
void configADC_ADCSet12BitResolution(void);
void configADC_SetTimerTrigger(void);
void configADC_SetProcessorTrigger(void);
void configADC_TimerConfig(void);
void configADC_TimerStart(void);
void configADC_TimerStop(void);
void configADC_TimerSetFrequency(int f);



#endif /* CONFIGADC_H_ */
