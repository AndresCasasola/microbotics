
#ifndef TIVA_TASKS_H__
#define TIVA_TASKS_H__

#include "system_lib.h"
#include "bot_motion.h"

// Queue parameters
#define MAX_BUTTONS_QUEUE_ELEMENTS 1000
xQueueHandle buttonsQueue;
xQueueHandle portAQueue;

//uint16_t map_ADC51[] = {};
//uint16_t map_CM51[] = {};

#endif /*TIVA_TASKS_H__ */
