//*****************************************************************************
//  Autores: Andres Casasola Dominguez, Juan Manuel Vazquez Jimenez.
//  Fecha: 2019/2020
//
//  Asignatura: Microbotica
//  Grado en Ingeniería de Sistemas Electronicos
//*****************************************************************************

#include "system_lib.h"
#include <tiva_tasks.h>
#include "bot_motion.h"

// Globals
uint32_t g_ui32CPUUsage;
uint32_t g_ulSystemClock;

// RTIs headers
void ButtonHandler(void);

// Utility Functions
void system_config();
void system_init();
void delayMS(uint32_t ms) {
    SysCtlDelay( (SysCtlClockGet()/(3*1000))*ms );
}

// Extern Functions
extern void ButtonsTask( void *pvParameters );
extern void ADCTask( void *pvParameters );
extern void GeneralTask( void *pvParameters );
extern void TimerTask( void *pvParameters );


int main(void){
    system_init();
//  -----
    UARTprintf("Hello! Tiva Working...\n");

    // Buttons Task
    if((xTaskCreate(ButtonsTask, (portCHAR *)"Buttons", 256, NULL, tskIDLE_PRIORITY + 1, NULL) != pdTRUE)) while(1);
    // ADC Task
    if((xTaskCreate(ADCTask, (portCHAR *)"ADC", 256, NULL, tskIDLE_PRIORITY + 1, NULL) != pdTRUE)) while(1);
    // General Task
    //if((xTaskCreate(GeneralTask, (portCHAR *)"General", 256, NULL, tskIDLE_PRIORITY + 1, NULL) != pdTRUE)) while(1);

    // Start scheduler
	vTaskStartScheduler();

	while(1);   //Si llego aqui es que algo raro ha pasado

}

void system_init(){
    system_config();
//  -----
    botStop();
    configADC_IniciaADC();
}

/********** System Configuration **********/

void system_config(){
    /*  ========================================================================  */
    /* =============== INICIO: Configuracion del sistema :INICIO ===============  */
    /*  ========================================================================  */

    // Set the clocking to run at 40 MHz from the PLL.
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); //Ponermos el reloj principal a 40 MHz (200 Mhz del Pll dividido por 5)

    // Get the system clock speed.
    g_ulSystemClock = SysCtlClockGet();

    //Habilita el clock gating de los perifericos durante el bajo consumo --> perifericos que se desee activos en modo Sleep
    //                                                                        deben habilitarse con SysCtlPeripheralSleepEnable
    ROM_SysCtlPeripheralClockGating(true);

    // Inicializa el subsistema de medida del uso de CPU (mide el tiempo que la CPU no esta dormida)
    // Para eso utiliza un timer, que aqui hemos puesto que sea el TIMER3 (ultimo parametro que se pasa a la funcion)
    // (y por tanto este no se deberia utilizar para otra cosa).
    CPUUsageInit(g_ulSystemClock, configTICK_RATE_HZ/10, 3);

    // Inicializa la UARTy la configura a 115.200 bps, 8-N-1 .
    //se usa para mandar y recibir mensajes y comandos por el puerto serie
    // Mediante un programa terminal como gtkterm, putty, cutecom, etc...

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0,115200,SysCtlClockGet());
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART0);   //La UART tiene que seguir funcionando aunque el micro este dormido
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOA);   //La UART tiene que seguir funcionando aunque el micro este dormido

    // Configuracion de botones e interrupciones
    ButtonsInit();
    IntRegister(INT_GPIOF, ButtonHandler);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_BOTH_EDGES);
    IntPrioritySet(INT_GPIOF,configMAX_SYSCALL_INTERRUPT_PRIORITY);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOF);
    GPIOIntEnable(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0);
    IntEnable(INT_GPIOF);
    IntMasterEnable();

    //Volvemos a configurar los LEDs en modo GPIO POR Defecto
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    /* ==================== Servos Configuration ==================== */

    uint32_t period = 12500; //20ms (40Mhz / 64pwm_divider)
//    uint32_t duty0 = 625;     //1ms pulse width
//    uint32_t duty90 = 940;     //1.5ms pulse width
//    uint32_t duty180 = 1250;     //2ms pulse width

    //Set the clock (Can't be changed, because then the Serial communication breaks down)
    //SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    int clock = SysCtlClockGet();
    //UARTprintf("Clock get: %d\n", clock);

    //Configure PWM Clock divide system clock by 64
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

   // Enable the peripherals used by this program.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);  //The Tiva Launchpad has two modules (0 and 1). Module 1 covers the LED pins
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_PWM1);


    //Configure PF1,PF2,PF3 Pins as PWM
    //GPIOPinConfigure(GPIO_PF1_M1PWM5);
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);

    //Configure PWM Options
    //PWM_GEN_2 Covers M1PWM4 and M1PWM5
    //PWM_GEN_3 Covers M1PWM6 and M1PWM7
    //PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    //Set the Period (expressed in clock ticks)
    //PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, period);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, period);

    // Enable the PWM generator
    //PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    // Turn on the Output pins
    //PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);    // Only PF1, PF2 and PF3

    IntMasterEnable();
}

/********** RTIs **********/

void ButtonHandler(void){

    GPIOIntDisable(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0);
    GPIOIntClear(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0);

//    BaseType_t xHigherPriorityTaskWoken;
    signed portBASE_TYPE xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    bool buttons[2];

    buttons[0]=( 0 == GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) );
    buttons[1]=( 0 == GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) );

    GPIOIntEnable(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0);

    if(xQueueSendFromISR(buttonsQueue, buttons, &xHigherPriorityTaskWoken) == errQUEUE_FULL){

        while(1);

    }
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

}




/*****  System Debug and Error Functions  *****/

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
// Esta funcion se llama si la biblioteca driverlib o FreeRTOS comprueban la existencia de un error (mediante
// las macros ASSERT(...) y configASSERT(...)
// Los parametros nombrefich y linea contienen informacion de en que punto se encuentra el error...
//
//*****************************************************************************
#ifdef DEBUG
void __error__(char *nombrefich, uint32_t linea)
{
//    while(1)
//    {
//    }
}
#endif
//*****************************************************************************
//
// Aqui incluimos los "ganchos" a los diferentes eventos del FreeRTOS
//
//*****************************************************************************
//Esto es lo que se ejecuta cuando el sistema detecta un desbordamiento de pila
//
void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}
//Esto se ejecuta cada Tick del sistema. LLeva la estadistica de uso de la CPU (tiempo que la CPU ha estado funcionando)
void vApplicationTickHook( void )
{
    static uint8_t count = 0;

    if (++count == 10)
    {
        g_ui32CPUUsage = CPUUsageTick();
        count = 0;
    }
    //return;
}
//Esto se ejecuta cada vez que entra a funcionar la tarea Idle
void vApplicationIdleHook (void)
{
    SysCtlSleep();
}
//Esto se ejecuta cada vez que entra a funcionar la tarea Idle
void vApplicationMallocFailedHook (void)
{
    while(1);
}


