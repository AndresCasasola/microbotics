
#include "tiva_tasks.h"

// Globales
TaskHandle_t xHandle=NULL;  // Manejador de la tarea para poder eliminarla


portTASK_FUNCTION( ButtonsTask, pvParameters ){

    if ((buttonsQueue = xQueueCreate(MAX_BUTTONS_QUEUE_ELEMENTS, 2*sizeof(bool))) == NULL ){
        while(1);
    }
    bool buttons[2];
    while(1){
        xQueueReceive(buttonsQueue, (void *) buttons, portMAX_DELAY);
        if (buttons[0]){

        }
        if (buttons[1]){

        }
    }
}

portTASK_FUNCTION(ADCTask,pvParameters){
    MuestrasADC muestras;
    configADC_SetTimerTrigger();
    configADC_TimerConfig();
    configADC_TimerStart();
    configADC_TimerSetFrequency(1);

    while(1){
        configADC_LeeADC(&muestras);
        uint16_t ch1 = muestras.chan1;  // PE3
        uint16_t ch2 = muestras.chan2;  // PE2
        uint16_t ch3 = muestras.chan3;  // PE1
        uint16_t ch4 = muestras.chan4;  // PE0
        UARTprintf("Samples: [PE3: %d] [PE2: %d] [PE1: %d] [PE0: %d]\n", ch1, ch2, ch3, ch4);
    }
}

portTASK_FUNCTION(GeneralTask,pvParameters){}

