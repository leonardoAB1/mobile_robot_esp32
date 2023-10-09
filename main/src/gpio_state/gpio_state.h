/*******************************************************************************
 * @file        gpio_state.h
 * @author      Leonardo Acha Boiano
 * @date        22 Jun 2023
 * 
 * @note        This code is written in C and is used on an ESP32 DEV KIT V1 board.
 *
 *******************************************************************************/
#ifndef GPIO_STATE_H_
#define GPIO_STATE_H_

#include "../gpio_utils/gpio_utils.h"
#include "../logging/logging_utils.h"
#include <stdint.h>

// Function to check the GPIO's current state
void checkGPIOState(uint8_t currentState, volatile uint8_t* stateVariable);

// Setter function
void setReference1State(float_t state);
// Getter functions
float_t getReference1State(void);
// Setter function
void setReference2State(float_t state);
// Getter functions
float_t getReference2State(void);

#endif /* GPIO_STATE_H_ */

/********************************* END OF FILE ********************************/
/******************************************************************************/
