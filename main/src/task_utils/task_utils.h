/*******************************************************************************
 * @file        task_utils.h
 * @author      Leonardo Acha Boiano
 * @date        13 Jun 2023
 * 
 * @note        This code is written in C and is used on an ESP32-CAM development board.
 *
 *******************************************************************************/
#ifndef _TASKS_H_
#define _TASKS_H_

#include "stdio.h"
#include "esp_task_wdt.h"
#include "esp_timer.h"

#include "../motor_control/motor_control.h"
#include "../encoder/encoder.h"
#include "../logging/logging_utils.h"
#include "../http_handlers/http_handlers.h"

#ifdef ENABLE_BLE
#include "../ble_utils/ble_utils.h"
#endif

// Define the stack depth and priority for the Default Control Task
#define TASK_MOTOR_DEFAULT_CONTROL_STACK_DEPTH 2048*3
#define TASK_MOTOR_DEFAULT_CONTROL_PRIORITY tskIDLE_PRIORITY+10
#define TASK_MOTOR_DEFAULT_CONTROL_CORE 1

#if ENABLE_BLE
// Define the stack depth and priority for the BLE Notification Task
#define TASK_BLE_NOTIFICATION_STACK_DEPTH 1024*5 
#define TASK_BLE_NOTIFICATION_PRIORITY tskIDLE_PRIORITY+11
#define TASK_BLE_NOTIFICATION_CORE 0
#endif /* ENABLE_BLE */

// Define the stack depth and priority for the Default Control Task
#define ENCODER1_STACK_DEPTH 2048*3
#define TASK_ENCODER1_PRIORITY tskIDLE_PRIORITY+8
#define ENCODER1_CORE 0
#define LOG_INTERVAL_MS 1

#define MOTOR_ANGLES_QUEUE_ITEM_NUMBER 10  

#define ALPHA 0.1 // Smoothing factor for the EWMA
#define THRESHOLD 0.9 // Threshold for comparing with angle

//Struct to pass angles to Queue
typedef struct
{
    uint8_t angle1;
    uint8_t angle2;
} MotorAngles_t;

//Declare task params structure
typedef struct
{
    QueueHandle_t param1;
} TaskParams_t;

/**
 * @brief Task configuration structure used to create a task configuration table.
 * Note: this is for dynamic memory allocation. We create all the tasks up front
 * dynamically and then never allocate memory again after initialization.
 */
typedef struct
{
	TaskFunction_t const TaskCodePtr;		 /*< Pointer to the task function */
	const char *const TaskName;				 /*< String task name             */
	const configSTACK_DEPTH_TYPE StackDepth; /*< Stack depth                  */
	const void *const ParametersPtr;		 /*< Parameter Pointer            */
	UBaseType_t TaskPriority;				 /*< Task Priority                */
	TaskHandle_t *const TaskHandle;			 /*< Pointer to task handle       */
	uint8_t TaskCore;					     /*< Task core (0 or 1)           */
} TaskInitParams_t;

/**
 * @brief Initializes the tasks and creates the task table.
 * 
 */
void initialize_tasks(void);

void MotorDefaultControlTask(void *pvParameters);

void Encoder1ProcessingTask(void *pvParameters);

extern Motor motor1;
extern Motor motor2;
extern float_t referenceState;
extern Encoder encoder1;
extern uint8_t ControlStrategy;

#endif /* _TASKS_H_ */

/********************************* END OF FILE ********************************/
/******************************************************************************/
