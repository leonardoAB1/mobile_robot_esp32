/*******************************************************************************
 * @file        gpio_state.c
 * @author      Leonardo Acha Boiano
 * @date        22 Jun 2023
 * 
 * @note        This code is written in C and is used on an ESP32 DEV KIT V1 board.
 *
 *******************************************************************************/
#include "../gpio_state/gpio_state.h"

volatile float_t referenceState1 = 0;
volatile float_t referenceState2 = 0;
volatile float_t robot_speed_fk = 0;
volatile float_t robot_angular_speed_fk = 0;
volatile float_t robot_x = 0;
volatile float_t robot_y = 0;
volatile float_t robot_theta = 0;

// Function to check the GPIO's current state
void checkGPIOState(uint8_t currentState, volatile uint8_t* stateVariable) {
    switch (currentState) {
        case 0:
            *stateVariable = 0;  // GPIO state changed from 1 to 0
            break;
        case 1:
            *stateVariable = 1;  // GPIO state changed from 0 to 1
            break;
    }
}

// Getter functions for input reference's state AKA reference
float_t getReference1State(void) {
    return referenceState1;
}

// Setter functions for input reference's state AKA reference
void setReference1State(float_t state) {
    referenceState1 = state;
}

// Getter functions for input reference's state AKA reference
float_t getReference2State(void) {
    return referenceState2;
}

// Setter functions for input reference's state AKA reference
void setReference2State(float_t state) {
    referenceState2 = state;
}

float_t getAngularSpeedState(void){
    return robot_angular_speed_fk;
}

void setSpeedState(float_t state){
    robot_speed_fk=state;
}

float_t getSpeedState(void){
    return robot_speed_fk;
}

void setAngularSpeedState(float_t state){
    robot_angular_speed_fk=state;
}

float_t getRobotXState(void){
    return robot_x;
}

void resetRobotXState(void){
    robot_x=0;
}

void setRobotXState(float_t state){
    robot_x=state;
}

float_t getRobotYState(void){
    return robot_y;
}

void resetRobotYState(void){
    robot_y=0;
}

void setRobotYState(float_t state){
    robot_y=state;
}

float_t getRobotThetaState(void){
    return robot_theta;
}

void resetRobotThetaState(void){
    robot_theta=0;
}

void setRobotThetaState(float_t state){
    robot_theta=state;
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
