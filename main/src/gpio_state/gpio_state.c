/*******************************************************************************
 * @file        gpio_state.c
 * @author      Leonardo Acha Boiano
 * @date        22 Jun 2023
 * 
 * @note        This code is written in C and is used on an ESP32 DEV KIT V1 board.
 *
 *******************************************************************************/
#include "../gpio_state/gpio_state.h"

volatile float_t referenceState = 0;

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
float_t getReferenceState(void) {
    return referenceState;
}

// Setter functions for input reference's state AKA reference
void setReferenceState(float_t state) {
    referenceState = state;
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
