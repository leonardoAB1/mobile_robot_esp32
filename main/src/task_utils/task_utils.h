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

#if ENABLE_BLE
// Define the stack depth and priority for the BLE Notification Task
#define TASK_BLE_NOTIFICATION_STACK_DEPTH 1024*5 
#define TASK_BLE_NOTIFICATION_PRIORITY tskIDLE_PRIORITY+11
#define TASK_BLE_NOTIFICATION_CORE 0
#endif /* ENABLE_BLE */

// Define the stack depth and priority for the Motor1 Control Task
#define TASK_MOTOR1_CONTROL_STACK_DEPTH 2048*3
#define TASK_MOTOR1_CONTROL_PRIORITY tskIDLE_PRIORITY+8
#define TASK_MOTOR1_CONTROL_CORE 1
// Define the stack depth and priority for the Motor1 Control Task
#define ENCODER1_STACK_DEPTH 2048*3
#define TASK_ENCODER1_PRIORITY tskIDLE_PRIORITY+7
#define ENCODER1_CORE 1
// Define the stack depth and priority for the Motor1 Control Task
#define TASK_MOTOR2_CONTROL_STACK_DEPTH 2048*3
#define TASK_MOTOR2_CONTROL_PRIORITY tskIDLE_PRIORITY+8
#define TASK_MOTOR2_CONTROL_CORE 0
// Define the stack depth and priority for the Motor1 Control Task
#define ENCODER2_STACK_DEPTH 2048*3
#define TASK_ENCODER2_PRIORITY tskIDLE_PRIORITY+7
#define ENCODER2_CORE 0
// Define the stack depth and priority for the Direct Kinematics Task
#define DIRECT_KINEMATICS_STACK_DEPTH 2048*3
#define DIRECT_KINEMATICS_PRIORITY tskIDLE_PRIORITY+10
#define DIRECT_KINEMATICS_CORE 0

// Constants for the EWMA filter
#define EWMA_ALPHA 0.15 // Adjust this value to control the filter's smoothing effect
#define EWMA_ALPHA_2 0.2

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

typedef struct {
    QueueHandle_t motor1SpeedQueue;
    QueueHandle_t motor2SpeedQueue;
} DirectKinematicsParams_t;

/**
 * @brief Initializes the tasks and creates the task table.
 * 
 */
void initialize_tasks(void);

void Motor1ControlTask(void *pvParameters);

void Encoder1ProcessingTask(void *pvParameters);

void Motor2ControlTask(void *pvParameters);

void Encoder2ProcessingTask(void *pvParameters);

void DirectKinematicsTask(void *pvParameters);

extern Motor motor1;
extern Motor motor2;
extern float_t referenceState1;
extern float_t referenceState2;
extern Encoder encoder1;
extern Encoder encoder2;
extern uint8_t ControlStrategy;

#endif /* _TASKS_H_ */

/********************************* END OF FILE ********************************/
/******************************************************************************/
