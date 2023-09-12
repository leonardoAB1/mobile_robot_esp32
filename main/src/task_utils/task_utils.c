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

SemaphoreHandle_t encoder1Binary;
SemaphoreHandle_t motor1Semaphore;
SemaphoreHandle_t encoder1Semaphore;

QueueHandle_t motor1SpeedQueue;

TaskParams_t taskParams;   // Define taskParams globally


void initialize_tasks(void)
{
	// Handle creation
	motor1Semaphore = xSemaphoreCreateMutex();
    encoder1Semaphore = xSemaphoreCreateMutex();
    encoder1Binary = xSemaphoreCreateBinary();

	// xQueueCreate(items in queue,  item's size in bytes)
    motor1SpeedQueue = xQueueCreate(50, sizeof(float));

    taskParams.param1 = motor1SpeedQueue;

	TaskInitParams_t const TaskInitParameters[] = {
		// Pointer to the Task function, Task String Name, The task stack depth, Parameter Pointer, Task priority, Task Handle
		{(TaskFunction_t)MotorDefaultControlTask, "motor_default_control_task", TASK_MOTOR_DEFAULT_CONTROL_STACK_DEPTH, &taskParams, TASK_MOTOR_DEFAULT_CONTROL_PRIORITY, &motorDefaultTask, TASK_MOTOR_DEFAULT_CONTROL_CORE},
        {(TaskFunction_t)Encoder1ProcessingTask, "encoder_processing_task", ENCODER1_STACK_DEPTH, &taskParams, TASK_ENCODER1_PRIORITY, &encoder1Task, ENCODER1_CORE}
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
    TaskParams_t* params = (TaskParams_t*) pvParameters;
    QueueHandle_t speedQueue = params->param1;

	uint8_t direction = 0;
	uint16_t duty_cycle = 0;
    //Parametros del controlador
    uint64_t prevTime = 0; 
    uint8_t fs= 10;
    float Ts= 1/fs;
    //////////////////////////
    uint16_t kp=1;
    uint16_t Td=0;
    uint16_t Ti=0;
    uint16_t q0=2*kp*Ti*Ts;
    uint16_t q1=Ts*2*kp;
    uint16_t q2=4*Td*Ti*kp;
    uint16_t q3=2*Ts*Ti;

    float e[3] = {0.0, 0.0, 0.0};; //control error   e[0]=e(k) e[1]=e(k-1) e[2]=e(k-2)
    float u[3] = {0.0, 0.0, 0.0};; //control output  u[0]=u(k) u[1]=u(k-1) u[2]=u(k-2)
    float y[2] = {0.0, 0.0};;      //speed           y[0]=y(k) y[1]=y(k-1)

	while (1) {
        //////////////////////////////////////////////////////////////////////////
        /////////////////////////////Open Loop System/////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //referenceState goes from 0 to 100 %
        if (ControlStrategy==OPEN_LOOP){
            if (referenceState >= 0 && referenceState <= 49) {
            direction = 1;
            // Map referenceState from 1 to (2**PWM_RESOLUTION) - 1
            float mappedValue = (float)referenceState / 49.0 * (pow(2, PWM_RESOLUTION) - 1);
            duty_cycle = (uint16_t)round(mappedValue);
            if (duty_cycle == 0) {
                duty_cycle = 1; // Ensure mapped value is at least 1
            }
            } else if (referenceState >= 51 && referenceState <= 100) {
                direction = 0; 
                // Map referenceState from 1 to (2**PWM_RESOLUTION) - 1
                float mappedValue = ((float)(referenceState - 51) / 49.0) * (pow(2, PWM_RESOLUTION) - 1);
                duty_cycle = (uint16_t)round(mappedValue);
                if (duty_cycle == 0) {
                    duty_cycle = 1; // Ensure mapped value is at least 1
                }
            } else if (referenceState == 50) {
                duty_cycle = 0;
            }

        //////////////////////////////////////////////////////////////////////////
        /////////////////////////////PID ISA Controller///////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //referenceState is a RPM value
        } else if (ControlStrategy==PID_CONTROLLER){
            uint64_t currentTime = esp_timer_get_time();
            if ((currentTime - prevTime)*1e-6 > Ts) {  //Refresh rate of the controller
                //check value of speed in encoder1 using get_speed(&encoder1, 0);
                xSemaphoreTake(encoder1Semaphore, portMAX_DELAY);
                if (xQueuePeek(speedQueue, &y[0], 0) == pdTRUE) {
                    xQueueReceive(speedQueue, &y[0], pdMS_TO_TICKS(100));
                }

                //Calculate error
                e[0] = referenceState - y[0];

                //PID ISA Controller
                u[0]=((q0+q1+q2)/q3)*e[0]+2*((q1-q2)/q3)*e[1]+((q2+q1-q0)/q3)*e[2]+u[2];
                
                //Control Signal Saturation
                if (u[0] > SERVO_FULL_DUTY) {
                    u[0] = SERVO_FULL_DUTY;
                }
                else if (u[0] < 0) {
                    u[0] = 0;
                }
                //Enviar señal de control
                duty_cycle = u[0];

                //Save actual data as previous data
                prevTime = currentTime;
                u[2] = u[1];
                e[2] = e[1];
                u[1] = u[0];
                e[1] = e[0];
                y[1] = y[0];

                xSemaphoreGive(encoder1Semaphore);
            }
        }
        //set duty cycle
        xSemaphoreTake(motor1Semaphore, portMAX_DELAY);
        move_motor(&motor1, duty_cycle, direction);
        xSemaphoreGive(motor1Semaphore);
        //log values
        if (xQueuePeek(speedQueue, &y[0], 0) == pdTRUE) {
            xQueueReceive(speedQueue, &y[0], pdMS_TO_TICKS(100));
        }

        ESP_LOGI(TASK_LOG_TAG, "SPEED:%f,REFERENCE:%f,CONTROL:%f,ERROR:%f", y[0], referenceState, u[0], e[0]);
        

        // Add a delay here if needed to control the task execution rate
        vTaskDelay(pdMS_TO_TICKS(100)); // Example: 10 ms delay
    }
}

void Encoder1ProcessingTask(void *pvParameters) {
    TaskParams_t* params = (TaskParams_t*) pvParameters;
    QueueHandle_t speedQueue = params->param1;

    // Initialize variables
    uint8_t prevPhaseA = get_phaseA(&encoder1);
    uint8_t prevPhaseB = get_phaseB(&encoder1);

    uint64_t prevTime = 0; 
    
    int32_t currentPosition  = 0;
    int32_t prevPosition = 0;

    uint64_t counter = 0;
    float speed = 0.0;

    while (1) {
        int8_t delta = 0;
        uint64_t currentTime = esp_timer_get_time(); // Use a hardware timer for time measurement
        if (xSemaphoreTake(encoder1Binary, portMAX_DELAY) == pdTRUE) {
            //////////////////////////////////////////////////////////////////////////
            /////////////////////////////update position//////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            xSemaphoreTake(encoder1Semaphore, portMAX_DELAY);
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
            counter += abs(delta);
            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////update speed////////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            // Calculate speed by counting the pulses during a second
            if (currentTime - prevTime >= 1e6){
                // Calculate position change
                speed = ((counter/((currentTime - prevTime)*1e-6))*60)/get_stepsPerRevolution(&encoder1); //((step/s)/(steps/rev))*(60s/1min)=rpm
                set_speed(&encoder1, speed);
                
                // Measure time using a hardware timer
                prevTime = currentTime;
                // Reboot step counter
                counter = 0;
                //////////////////////////////////////////////////////////////////////////
                /////////////////////////////Log Message//////////////////////////////////
                ESP_LOGI(TASK_LOG_TAG, "SPEED:%f RPM, POSITION:%f°", speed, get_positionDegrees(&encoder1));
                xSemaphoreGive(encoder1Semaphore);
                xQueueSend(speedQueue, &speed, 0);
                //////////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////
            }
            else{
                xSemaphoreGive(encoder1Semaphore);  
            }
        }
    }
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
