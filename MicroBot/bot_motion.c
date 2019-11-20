#include "bot_motion.h"

void botSetRight(uint8_t angle){
    //Set PWM duty
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, (180-angle)*3.47+625);//3.47 se corresponde con los ticks/angulo (mas el offset que obtenemos de 625 a 0º:(90º/0.5e-3)*12500)/20e-3
    delayMS(SET_DELAY);
}

void botSetLeft(uint8_t angle){
    //Set PWM duty
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, angle*3.47+625);//3.47 se corresponde con los ticks/angulo (mas el offset que obtenemos de 625 a 0º:(90º/0.5e-3)*12500)/20e-3
    delayMS(SET_DELAY);
}

void botGoStraight(uint8_t angle){
    botSetRight(angle);
    botSetLeft(angle);
}

void botTurnRightForward(uint8_t angle){    // Angle [90 - 180] - Maximo giro: 90
    botSetRight(angle);
    botSetLeft(180);
}

void botTurnLeftForward(uint8_t angle){     // Angle [90 - 180] - Maximo giro: 90
    botSetRight(180);
    botSetLeft(angle);
}

void botTurnRightBackward(uint8_t angle){        // Angle [0 - 90] - Maximo giro: 90
    botSetRight(0);
    botSetLeft(angle);
}

void botTurnLeftBackward(uint8_t angle){         // Angle [0 - 90] - Maximo giro: 90
    botSetRight(angle);
    botSetLeft(0);
}

void botStop(){
    botSetRight(RIGHT_STOP);
    botSetLeft(LEFT_STOP);
}

void botRotateRight(){
    botSetRight(0);
    botSetLeft(180);
}

void botRotateLeft(){
    botSetRight(180);
    botSetLeft(0);
}
