
#ifndef TIVA_TASKS_H__
#define TIVA_TASKS_H__

#include "system_lib.h"
#include "bot_motion.h"

// Queue parameters
#define MAX_BUTTONS_QUEUE_ELEMENTS 10
xQueueHandle buttonsQueue;

//uint16_t map_ADC41[] = {2885, 3493, 3009, 2543, 2252, 1985, 1780, 1582, 1433, 1228, 1060, 905, 725, 595, 483, 409, 347, 291};
//uint16_t map_CM41[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 20, 24, 28, 32, 36, 40};

//uint16_t map_ADC51[] = {};
//uint16_t map_CM51[] = {};

#endif /*TIVA_TASKS_H__ */
