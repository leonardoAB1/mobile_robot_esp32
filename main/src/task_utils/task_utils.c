/*******************************************************************************
 * @file        task_utils.c
 * @author      Leonardo Acha Boiano
 * @date        13 Jun 2023
 * @brief       This file contains the implementation of the tasks created in FreeRTOS
 * 
 * @note        This code is written in C and is used on an ESP32 DEV KIT V1 board.
 *
 *******************************************************************************/
#include "../task_utils/task_utils.h"

BaseType_t result;         // Define result globally

TaskHandle_t encoder1Task;    // Define task globally
TaskHandle_t motorDefaultTask;  // Define task globally

SemaphoreHandle_t motor1Semaphore;
SemaphoreHandle_t encoder1Semaphore;


QueueHandle_t motorAnglesQueue;

TaskParams_t taskParams;   // Define taskParams globally


void initialize_tasks(void)
{
	// Handle creation
	motor1Semaphore = xSemaphoreCreateMutex();
    encoder1Semaphore = xSemaphoreCreateBinary();

	// xQueueCreate(items in queue,  item's size in bytes)
	motorAnglesQueue = xQueueCreate(MOTOR_ANGLES_QUEUE_ITEM_NUMBER, sizeof(MotorAngles_t));

	taskParams.motorAnglesQueue = (MotorAngles_t*)motorAnglesQueue;

	TaskInitParams_t const TaskInitParameters[] = {
		// Pointer to the Task function, Task String Name, The task stack depth, Parameter Pointer, Task priority, Task Handle
		{(TaskFunction_t)MotorDefaultControlTask, "motor_default_control_task", TASK_MOTOR_DEFAULT_CONTROL_STACK_DEPTH, NULL, TASK_MOTOR_DEFAULT_CONTROL_PRIORITY, &motorDefaultTask, TASK_MOTOR_DEFAULT_CONTROL_CORE},
        {(TaskFunction_t)Encoder1ProcessingTask, "encoder_processing_task", ENCODER1_STACK_DEPTH, NULL, TASK_ENCODER1_PRIORITY, &encoder1Task, ENCODER1_CORE}
    };

	// Loop through the task table and create each task.
	for (size_t TaskCount = 0;
		 TaskCount < sizeof(TaskInitParameters) / sizeof(TaskInitParameters[0]);
		 TaskCount++)
	{
		result = xTaskCreatePinnedToCore(TaskInitParameters[TaskCount].TaskCodePtr,
							 TaskInitParameters[TaskCount].TaskName,
							 TaskInitParameters[TaskCount].StackDepth,
							 (void*)TaskInitParameters[TaskCount].ParametersPtr,
							 TaskInitParameters[TaskCount].TaskPriority,
							 TaskInitParameters[TaskCount].TaskHandle,
                             TaskInitParameters[TaskCount].TaskCore
                             );
		configASSERT(result == pdPASS); // Make sure the task was created successfully
	}	
    ESP_LOGI(TASK_LOG_TAG, "Tasks Initialized");
}

void MotorDefaultControlTask(void *pvParameters){
	uint8_t direction = 0;
	uint16_t mappedState = 0;

	while (1) {
        if (referenceState >= 0 && referenceState <= 49) {
            direction = 1;
            // Map referenceState from 1 to (2**PWM_RESOLUTION) - 1
            float mappedValue = (float)referenceState / 49.0 * (pow(2, PWM_RESOLUTION) - 1);
            mappedState = (uint16_t)round(mappedValue);
            if (mappedState == 0) {
                mappedState = 1; // Ensure mapped value is at least 1
            }

        } else if (referenceState >= 51 && referenceState <= 100) {
            direction = 0; 
            // Map referenceState from 1 to (2**PWM_RESOLUTION) - 1
            float mappedValue = ((float)(referenceState - 51) / 49.0) * (pow(2, PWM_RESOLUTION) - 1);
            mappedState = (uint16_t)round(mappedValue);
            if (mappedState == 0) {
                mappedState = 1; // Ensure mapped value is at least 1
            }

        } else if (referenceState == 50) {
            mappedState = 0;
        }
        //xSemaphoreTake(motor1Semaphore, portMAX_DELAY);
        move_motor(&motor1, mappedState, direction);
        //xSemaphoreGive(motor1Semaphore);
        //ESP_LOGI(TASK_LOG_TAG, "PWM:%u,DIRECTION:%u", mappedState, direction);

        // Add a delay here if needed to control the task execution rate
        vTaskDelay(pdMS_TO_TICKS(100)); // Example: 100 ms delay
    }
}

void Encoder1ProcessingTask(void *pvParameters) {
    // Initialize variables
    uint8_t prevPhaseA = get_phaseA(&encoder1);
    uint8_t prevPhaseB = get_phaseB(&encoder1);

    uint64_t prevTime = esp_timer_get_time(); 
    uint64_t elapsedTime = 0;
    
    int32_t currentPosition  = 0;
    int32_t prevPosition = 0;

    uint8_t counter = 0;

    while (1) {
        int8_t delta = 0;
        if (xSemaphoreTake(encoder1Semaphore, portMAX_DELAY) == pdTRUE) {
            //////////////////////////////////////////////////////////////////////////
            /////////////////////////////update position//////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            uint8_t PhaseA = gpio_get_level(FASE_A);
            uint8_t PhaseB = gpio_get_level(FASE_B);

            if (PhaseB != prevPhaseB && PhaseB == 1) {
                delta = (PhaseA != PhaseB) ? 1 : -1;
                set_direction(&encoder1, delta > 0 ? CW : CCW);
            }
            set_phaseA(&encoder1, PhaseA);
            set_phaseB(&encoder1, PhaseB);

            // Update position based on delta
            currentPosition = get_position(&encoder1) + delta;
            set_position(&encoder1, currentPosition);

            // Check for position limits
            int32_t stepsPerRevolution = get_stepsPerRevolution(&encoder1);
            if (currentPosition >= stepsPerRevolution || currentPosition <= -stepsPerRevolution) {
                set_position(&encoder1, 0);
            }
            prevPhaseA = PhaseA;
            prevPhaseB = PhaseB;
            counter += delta;
            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////update speed////////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            if ((delta!=0)&(counter>=2)){
                // Measure time using a hardware timer
                uint64_t currentTime = esp_timer_get_time(); // Use a hardware timer for time measurement
                elapsedTime = currentTime - prevTime;
                prevTime = currentTime;

                // Calculate position change
                float positionChange = fabs(currentPosition - prevPosition);
                float speed = (positionChange * get_degreesPerStep(&encoder1) * 1000000 * 60) / (elapsedTime * 360);
                set_speed(&encoder1, speed);

                // Update the previous position
                prevPosition = currentPosition; 
                //////////////////////////////////////////////////////////////////////////
                /////////////////////////////Log Message//////////////////////////////////
                ESP_LOGI(TASK_LOG_TAG, "SPEED:%f,POSITION:%f", get_speed(&encoder1), get_positionDegrees(&encoder1));
                //////////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////
            }
            else{
                set_speed(&encoder1, 0);
            }
        }
    }
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
