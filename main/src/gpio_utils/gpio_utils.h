/*******************************************************************************
 * @file        gpio_utils.h
 * @author      Leonardo Acha Boiano
 * @date        11 Jun 2023
 * 
 * @note        This code is written in C and is used on an ESP32 DEV KIT V1 board.
 *
 *******************************************************************************/
#ifndef GPIO_UTILS_H_
#define GPIO_UTILS_H_

#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "../logging/logging_utils.h"
#include "../motor_control/motor_control.h"
#include "../encoder/encoder.h"

#define HIGH    1
#define LOW     0

enum gpio_num_t{
    // Safe to use pins (Your top priority pins)
    GPIO_23 = 36,
    GPIO_22 = 39,
    GPIO_21 = 42,
    GPIO_19 = 38,
    GPIO_18 = 35,
    GPIO_17 = 27,
    GPIO_16 = 25,
    GPIO_04 = 24,
    GPIO_32 = 12,
    GPIO_33 = 13,
    GPIO_25 = 14,
    GPIO_26 = 15,
    GPIO_27 = 16,
    GPIO_14 = 17,
    GPIO_13 = 20,

    // Caution pins (Pay close attention)
    GPIO_05 = 34,
    GPIO_02 = 22,
    GPIO_15 = 21,
    GPIO_12 = 18,

    // Avoid pins (It is recommended to avoid using these pins)
    GPIO_01 = 41,
    GPIO_03 = 40,

    // Input-only GPIO pins (Cannot be configured as output)
    GPIO_34 = 10,
    GPIO_35 = 11,
    GPIO_36 = 5,
    GPIO_39 = 8

/** @endcond */
};

//ENCODER 1 GM25-370
#define FASE_A1 GPIO_NUM_23
#define FASE_B1 GPIO_NUM_22 

//ENCODER 2 GM25-370
#define FASE_A2 GPIO_NUM_21
#define FASE_B2 GPIO_NUM_19 

//PRIMER MOTOR
#define GPIO_ENA GPIO_NUM_14
#define GPIO_IN1 GPIO_NUM_27
#define GPIO_IN2 GPIO_NUM_26

//SEGUNDO MOTOR
#define GPIO_IN3 GPIO_NUM_25
#define GPIO_IN4 GPIO_NUM_33
#define GPIO_ENB GPIO_NUM_32

/**
 * @brief GPIO Initialization.
 */
esp_err_t init_gpio(void);

/**
 * @brief Interrupt configuration structure used to initialize interrupts.
 */
typedef struct
{
    gpio_num_t gpioNum;                /*< GPIO number for the interrupt pin */
    gpio_isr_t isrHandler;             /*< Pointer to the interrupt service routine */
    void *userData;                    /*< User data to be passed to the ISR */
    gpio_int_type_t interruptType;     /*< Interrupt type (e.g., GPIO_INTR_ANYEDGE) */
} InterruptInitParams_t;

/**
 * @brief Initializes the timer.
 */
esp_err_t init_timers(void);

void vTimer1Callback( TimerHandle_t pxTimer );
void vTimer2Callback( TimerHandle_t pxTimer );

/**
 * @brief Initializes the interrupts.
 */
esp_err_t init_isr(void);

// Function declarations for the interrupt service routines
void update_encoder1_isr(void* arg);
void update_encoder2_isr(void* arg);

extern Encoder encoder1;
extern SemaphoreHandle_t encoder1Binary;

extern Encoder encoder2;
extern SemaphoreHandle_t encoder2Binary;

#endif /* GPIO_UTILS_H_ */

/********************************* END OF FILE ********************************/
/******************************************************************************/
