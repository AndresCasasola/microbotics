#ifndef BOT_MOTION_H_
#define BOT_MOTION_H_

#include <stdbool.h>
#include <stdint.h>
#include "driverlib/pwm.h"
#include "inc/hw_memmap.h"

#define RIGHT_STOP  90
#define LEFT_STOP   89
#define SET_DELAY   10  // ms

void botSetRight(uint8_t angle);
void botSetLeft(uint8_t angle);
void botGoStraight(uint8_t angle);
void botTurnRightForward(uint8_t angle);
void botTurnLeftForward(uint8_t angle);
void botTurnRightBackward(uint8_t angle);
void botTurnLeftBackward(uint8_t angle);
void botStop();
void botRotateRight();
void botRotateLeft();


#endif /* BOT_MOTION_H_ */
