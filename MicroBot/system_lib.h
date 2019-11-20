
// C Libraries
#include <stdbool.h>
#include <stdint.h>
#include "stdlib.h"
#include <string.h>

// HW libraries
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_uart.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"

// TivaWare
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/debug.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// Others libraries
#include "drivers/rgb.h"
#include "drivers/configADC.h"
#include "drivers/buttons.h"
#include "utils/cpu_usage.h"
#include "utils/uartstdio.h"
