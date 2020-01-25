
#include "tiva_tasks.h"

#define BOT_SPEED       180
#define BOT_STOP_SPEED  90

// Functions
void SensorTask( void *pvParameters );

// Globales
TaskHandle_t xHandle = NULL;  // Manejador de la tarea para poder eliminarla
uint16_t map_41ADC[] = {3493, 3009, 2543, 2252, 1985, 1780, 1582, 1433, 1228, 1060, 905, 725, 595, 483, 409, 347, 291};
uint16_t map_41CM[] = {3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 20, 24, 28, 32, 36, 40};
uint16_t d = 10;

portTASK_FUNCTION(ReactiveTask, pvParameters ){

    // Start bot
    botStop();
    botGoStraight(BOT_SPEED);

    if ((portAQueue = xQueueCreate(MAX_BUTTONS_QUEUE_ELEMENTS, 3*sizeof(bool))) == NULL ){
        while(1);
    }
    bool buttons[3];

    // Run SensorTask with xHandle handler
    if((xTaskCreate(SensorTask, (portCHAR *)"General", 256, NULL, tskIDLE_PRIORITY + 1, &xHandle) != pdTRUE)) while(1);

    while(1){

        // Waiting for encoder data
        xQueueReceive(portAQueue, (void *) buttons, portMAX_DELAY);

        // Action
        if (buttons[1] == 0){           // PA3
            //UARTprintf("Boton2\n");   // Debug

            // Kill SensorTask
            vTaskDelete(xHandle);

            botStop();
            botRotateRight();
            SysCtlDelay( (SysCtlClockGet()/(3*1000))*1070 );    // Rotate 180 degrees
            botGoStraight(BOT_SPEED);

            // Run SensorTask with xHandle handler
            if((xTaskCreate(SensorTask, (portCHAR *)"General", 256, NULL, tskIDLE_PRIORITY + 1, &xHandle) != pdTRUE)) while(1);
        }

        xQueueReset(portAQueue);
    }
}

portTASK_FUNCTION(SensorTask,pvParameters){

    // Configure ADC
    MuestrasADC muestras;
    configADC_SetTimerTrigger();
    configADC_TimerConfig();
    configADC_TimerStart();
    configADC_TimerSetFrequency(10);

    // Reset ADC queue
    configADC_ResetQueue();

    while(1){

        // Start bot
        botGoStraight(BOT_SPEED);

        // Read ADC
        configADC_LeeADC(&muestras);
        uint16_t ch1 = muestras.chan1;      // PE3
        //uint16_t ch2 = muestras.chan2;    // PE2
        //uint16_t ch3 = muestras.chan3;    // PE1
        //uint16_t ch4 = muestras.chan4;    // PE0

        // Calculate distance
        uint8_t n = 0;
        while(ch1 < map_41ADC[n]){
            n++;                    // Ahora n tiene el valor de la posicion inferior del array (i-1 es el superior)
        }
        uint8_t big = 0;            // Bigger value
        //uint8_t sma = 0;          // Smaller value
        big = map_41CM[n];
        //sma = map_41CM[n-1];

        if (n > 16){
            big = 100;
        }

        // Action

//        if(big <= (map_41CM[5])){       // 8 cm
//            botStop();
//            botRotateRight();
//            SysCtlDelay( (SysCtlClockGet()/(3*1000))*1070 );    // Rotate 180 degrees
//            botStop();
//        }

        if(big <= (map_41CM[8])){       // 12 cm
            botTurnRightForward(97);
            SysCtlDelay( (SysCtlClockGet()/(3*1000))*1600 );    // Sleep
            botTurnLeftForward(97);
            SysCtlDelay( (SysCtlClockGet()/(3*1000))*2000 );    // Sleep
            botRotateLeft();
            SysCtlDelay( (SysCtlClockGet()/(3*1000))*800 );     // Sleep
            //botStop();
            botGoStraight(BOT_SPEED);
        }

        //if(big <= (map_41CM[11])){      // 20 cm
            //botStop();
            //SysCtlDelay( (SysCtlClockGet()/(3*1000))*500 );     // Sleep
        //}

        // Reset ADC queue
        configADC_ResetQueue();
    }
}

portTASK_FUNCTION( ButtonsTask, pvParameters ){

    if ((buttonsQueue = xQueueCreate(MAX_BUTTONS_QUEUE_ELEMENTS, 2*sizeof(bool))) == NULL ){
        while(1);
    }
    bool buttons[2];
    while(1){
        xQueueReceive(buttonsQueue, (void *) buttons, portMAX_DELAY);
        if (buttons[0]){        // SW1
            botStop();
        }
        if (buttons[1]){        // SW2
            botStop();
        }
    }
}

portTASK_FUNCTION( PortATask, pvParameters ){

    uint16_t counter = 0;
    uint16_t speed = 0;

    botGoStraight(speed);
    //botStop();

    if ((portAQueue = xQueueCreate(MAX_BUTTONS_QUEUE_ELEMENTS, 3*sizeof(bool))) == NULL ){
        while(1);
    }
    bool buttons[3];
    while(1){
        xQueueReceive(portAQueue, (void *) buttons, portMAX_DELAY);
        if (buttons[0] == 0){        // PA2
            counter++;
            UARTprintf("Encoder: %d\n", counter);
            if((0 <= counter) && (counter < 18)){
                botGoStraight(speed);
                UARTprintf("1: %d\n", counter);
            }
            if(18 <= counter && counter < 33){
                botTurnLeftForward(90);
                UARTprintf("2: %d\n", counter);
            }
            if(33 <= counter && counter < 45){
                botGoStraight(speed);
                UARTprintf("3: %d\n", counter);
            }
            if(45 <= counter && counter < 60){
                botTurnLeftForward(90);
                UARTprintf("4: %d\n", counter);
            }
            if(60 <= counter && counter < 78){
                botGoStraight(speed);
                UARTprintf("5: %d\n", counter);
            }
            if(78 <= counter && counter < 93){
                botTurnLeftForward(90);
                UARTprintf("6: %d\n", counter);
            }
            if(93 <= counter && counter < 105){
                botGoStraight(speed);
                UARTprintf("7: %d\n", counter);
            }
            if(105 <= counter && counter < 120){
                botTurnLeftForward(90);
                UARTprintf("8: %d\n", counter);
            }
            if(counter >= 120){
                botStop();
                UARTprintf("8: %d\n", counter);
            }
            SysCtlDelay( (SysCtlClockGet()/(3*1000))*10 );

        }
        if (buttons[1] == 0){            // PA3
            UARTprintf("Boton2\n");
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

