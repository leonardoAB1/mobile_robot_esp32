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
#define Ts 1e-3
#define Kp 0.228
#define Ki 2.1
#define Kd 0.0001

BaseType_t result;         // Define result globally

TaskHandle_t encoder1Task;    // Define task globally
TaskHandle_t motor1Task;  // Define task globally
SemaphoreHandle_t encoder1Binary;
SemaphoreHandle_t motor1Semaphore;
SemaphoreHandle_t encoder1Semaphore;
QueueHandle_t motor1SpeedQueue;
TaskParams_t taskParams1;   // Define taskParams globally

TaskHandle_t encoder2Task;
TaskHandle_t motor2Task;
SemaphoreHandle_t encoder2Binary;
SemaphoreHandle_t motor2Semaphore;
SemaphoreHandle_t encoder2Semaphore;
QueueHandle_t motor2SpeedQueue;
TaskParams_t taskParams2; 

DirectKinematicsParams_t directKinematicsParams;

void initialize_tasks(void)
{
	// Handle creation
	motor1Semaphore = xSemaphoreCreateMutex();
    encoder1Semaphore = xSemaphoreCreateMutex();
    encoder1Binary = xSemaphoreCreateBinary();
    motor1SpeedQueue = xQueueCreate(50, sizeof(float));
    taskParams1.param1 = motor1SpeedQueue;

    motor2Semaphore = xSemaphoreCreateMutex();
    encoder2Semaphore = xSemaphoreCreateMutex();
    encoder2Binary = xSemaphoreCreateBinary();
    motor2SpeedQueue = xQueueCreate(50, sizeof(float));
    taskParams2.param1 = motor2SpeedQueue;

    directKinematicsParams.motor1SpeedQueue = motor1SpeedQueue;
    directKinematicsParams.motor2SpeedQueue = motor2SpeedQueue;

	TaskInitParams_t const TaskInitParameters[] = {
		// Pointer to the Task function, Task String Name, The task stack depth, Parameter Pointer, Task priority, Task Handle
		{(TaskFunction_t)Motor1ControlTask, "motor1_control_task", TASK_MOTOR1_CONTROL_STACK_DEPTH, &taskParams1, TASK_MOTOR1_CONTROL_PRIORITY, &motor1Task, TASK_MOTOR1_CONTROL_CORE},
        {(TaskFunction_t)Encoder1ProcessingTask, "encoder_processing_task", ENCODER1_STACK_DEPTH, &taskParams1, TASK_ENCODER1_PRIORITY, &encoder1Task, ENCODER1_CORE},
        {(TaskFunction_t)Motor2ControlTask, "motor2_control_task", TASK_MOTOR2_CONTROL_STACK_DEPTH, &taskParams2, TASK_MOTOR2_CONTROL_PRIORITY, &motor2Task, TASK_MOTOR2_CONTROL_CORE},
        {(TaskFunction_t)Encoder2ProcessingTask, "encoder_processing_task", ENCODER2_STACK_DEPTH, &taskParams2, TASK_ENCODER2_PRIORITY, &encoder2Task, ENCODER2_CORE},
        {(TaskFunction_t)DirectKinematicsTask, "direct_kinematics_task", DIRECT_KINEMATICS_STACK_DEPTH, &directKinematicsParams, DIRECT_KINEMATICS_PRIORITY, NULL, DIRECT_KINEMATICS_CORE}
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

float_t filterEWMA(float_t newValue, float_t oldValue, float_t alpha) {
    return alpha * newValue + (1 - alpha) * oldValue;
}

void Motor1ControlTask(void *pvParameters) {
    TaskParams_t* params = (TaskParams_t*) pvParameters;
    QueueHandle_t speedQueue = params->param1;

    uint8_t direction = 0;
    uint16_t duty_cycle = 0;
    uint8_t counter = 0;

    // Controller parameters
    uint64_t prevTime = 0;

    // Control variables
    float_t e[3] = {0.0, 0.0, 0.0};
    float_t u[3] = {0.0, 0.0, 0.0};
    float_t y[2] = {0.0, 0.0};

    while (1) {
        uint64_t currentTime = esp_timer_get_time();

        xSemaphoreTake(encoder1Semaphore, portMAX_DELAY);
        if (xQueuePeek(speedQueue, &y[0], 0) == pdTRUE) {
            xQueueReceive(speedQueue, &y[0], pdMS_TO_TICKS(100));
        }
        xSemaphoreGive(encoder1Semaphore);
        // Apply EWMA filter
        y[0] = filterEWMA(y[0], y[1], EWMA_ALPHA);
        y[0] = filterEWMA(y[0], y[1], EWMA_ALPHA_2);
        y[0] = roundToDecimal(y[0], 0);

        // Map referenceState1 to duty_cycle for open-loop control
        if (ControlStrategy == OPEN_LOOP) {
            if (referenceState1 >= 0 && referenceState1 <= 100) {
                if (referenceState1 == 50) {
                    duty_cycle = 0;
                } else {
                    direction = (referenceState1 <= 49) ? 1 : 0;
                    // Map referenceState1 from 1 to (2^PWM_RESOLUTION) - 1
                    float mappedValue = ((float)(referenceState1 - (direction ? 0 : 51)) / 49.0) * (pow(2, PWM_RESOLUTION) - 1);
                    duty_cycle = (uint16_t)round(mappedValue);
                    if (duty_cycle < 1) {
                        duty_cycle = 1; // Ensure mapped value is at least 1
                    }
                }
            }
        }
        // PID Controller for closed-loop control
        else if (ControlStrategy == PID_CONTROLLER) {
            if ((currentTime - prevTime) * 1e-6 > Ts) {  // Refresh rate of the controller
                direction = (referenceState1 <= 0) ? 1 : 0;
                
                // Calculate error
                e[0] = fabs(referenceState1) - y[0];

                // PID Controller
                u[0] = u[1] + Kp * (e[0] - e[1]) + Ki * Ts * e[0] + (Kd / Ts) * (e[0] - 2 * e[1] + e[2]);

                // Control Signal Saturation
                if (u[0] > 100) {
                    u[0] = 100;
                } else if (u[0] < 0) {
                    u[0] = 0;
                }

                // Calculate duty_cycle
                duty_cycle = (u[0] / 100) * SERVO_FULL_DUTY;
            }
        }

        // Save actual data as previous data
        prevTime = currentTime; // Store the current time regardless of control strategy
        u[2] = u[1];
        e[2] = e[1];
        u[1] = u[0];
        e[1] = e[0];
        y[1] = y[0];

        // Set duty cycle
        xSemaphoreTake(motor1Semaphore, portMAX_DELAY);
        move_motor(&motor1, duty_cycle, direction);
        xSemaphoreGive(motor1Semaphore);

        y[0]= (direction==0)? y[0] : -y[0];

        // Log values
        counter+=1;
        if (counter==10)
        {
            ESP_LOGI("MOTOR 1:", "SPEED:%f,REFERENCE:%f,CONTROL:%f,ERROR:%f", y[0], referenceState1, u[0], e[0]);
            counter=0;
        }
        
        // Delay for task execution rate control
        vTaskDelay(pdMS_TO_TICKS(1)); // 1 ms delay
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
            uint8_t PhaseA = gpio_get_level(FASE_A1);
            uint8_t PhaseB = gpio_get_level(FASE_B1);

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
            // Calculate speed by counting the pulses during 1 milli second
            if (currentTime - prevTime >= 1e3){
                // Calculate position change
                speed = (((counter*1e6)/(currentTime - prevTime))*60)/get_stepsPerRevolution(&encoder1); //((step/s)/(steps/rev))*(60s/1min)=rpm
                set_speed(&encoder1, speed);
                
                // Measure time using a hardware timer
                prevTime = currentTime;
                // Reboot step counter
                counter = 0;
                //////////////////////////////////////////////////////////////////////////
                /////////////////////////////Log Message//////////////////////////////////
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

void Motor2ControlTask(void *pvParameters) {
    TaskParams_t* params = (TaskParams_t*) pvParameters;
    QueueHandle_t speedQueue = params->param1;

    uint8_t direction = 0;
    uint16_t duty_cycle = 0;
    uint8_t counter = 0;

    // Controller parameters
    uint64_t prevTime = 0;

    // Control variables
    float_t e[3] = {0.0, 0.0, 0.0};
    float_t u[3] = {0.0, 0.0, 0.0};
    float_t y[2] = {0.0, 0.0};

    while (1) {
        uint64_t currentTime = esp_timer_get_time();

        xSemaphoreTake(encoder1Semaphore, portMAX_DELAY);
        if (xQueuePeek(speedQueue, &y[0], 0) == pdTRUE) {
            xQueueReceive(speedQueue, &y[0], pdMS_TO_TICKS(100));
        }
        xSemaphoreGive(encoder1Semaphore);
        // Apply EWMA filter
        y[0] = filterEWMA(y[0], y[1], EWMA_ALPHA);
        y[0] = filterEWMA(y[0], y[1], EWMA_ALPHA_2);
        y[0] = roundToDecimal(y[0], 0);

        // Map referenceState2 to duty_cycle for open-loop control
        if (ControlStrategy == OPEN_LOOP) {
            if (referenceState2 >= 0 && referenceState2 <= 100) {
                if (referenceState2 == 50) {
                    duty_cycle = 0;
                } else {
                    direction = (referenceState2 <= 49) ? 1 : 0;
                    // Map referenceState1 from 1 to (2^PWM_RESOLUTION) - 1
                    float mappedValue = ((float)(referenceState2 - (direction ? 0 : 51)) / 49.0) * (pow(2, PWM_RESOLUTION) - 1);
                    duty_cycle = (uint16_t)round(mappedValue);
                    if (duty_cycle < 1) {
                        duty_cycle = 1; // Ensure mapped value is at least 1
                    }
                }
            }
        }
        // PID Controller for closed-loop control
        else if (ControlStrategy == PID_CONTROLLER) {
            if ((currentTime - prevTime) * 1e-6 > Ts) {  // Refresh rate of the controller
                direction = (referenceState2 <= 0) ? 1 : 0;
                
                // Calculate error
                e[0] = fabs(referenceState2) - y[0];

                // PID Controller
                u[0] = u[1] + Kp * (e[0] - e[1]) + Ki * Ts * e[0] + (Kd / Ts) * (e[0] - 2 * e[1] + e[2]);

                // Control Signal Saturation
                if (u[0] > 100) {
                    u[0] = 100;
                } else if (u[0] < 0) {
                    u[0] = 0;
                }

                // Calculate duty_cycle
                duty_cycle = (u[0] / 100) * SERVO_FULL_DUTY;
            }
        }

        // Save actual data as previous data
        prevTime = currentTime; // Store the current time regardless of control strategy
        u[2] = u[1];
        e[2] = e[1];
        u[1] = u[0];
        e[1] = e[0];
        y[1] = y[0];

        // Set duty cycle
        xSemaphoreTake(motor2Semaphore, portMAX_DELAY);
        move_motor(&motor2, duty_cycle, direction);
        xSemaphoreGive(motor2Semaphore);

        y[0]= (direction==0)? y[0] : -y[0];

        // Log values
        counter+=1;
        if (counter==10)
        {
            ESP_LOGI("MOTOR 2:", "SPEED:%f,REFERENCE:%f,CONTROL:%f,ERROR:%f", y[0], referenceState2, u[0], e[0]);
            counter=0;
        }
        
        // Delay for task execution rate control
        vTaskDelay(pdMS_TO_TICKS(1)); // 1 ms delay
    }
}

void Encoder2ProcessingTask(void *pvParameters) {
    TaskParams_t* params = (TaskParams_t*) pvParameters;
    QueueHandle_t motor2SpeedQueue = params->param1;

    // Initialize variables
    uint8_t prevPhaseA = get_phaseA(&encoder2);
    uint8_t prevPhaseB = get_phaseB(&encoder2);

    uint64_t prevTime = 0; 
    
    int32_t currentPosition  = 0;
    int32_t prevPosition = 0;

    uint64_t counter = 0;
    float speed = 0.0;

    while (1) {
        int8_t delta = 0;
        uint64_t currentTime = esp_timer_get_time(); // Use a hardware timer for time measurement
        if (xSemaphoreTake(encoder2Binary, portMAX_DELAY) == pdTRUE) {
            //////////////////////////////////////////////////////////////////////////
            /////////////////////////////update position//////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            xSemaphoreTake(encoder2Semaphore, portMAX_DELAY);
            uint8_t PhaseA = gpio_get_level(FASE_A1);
            uint8_t PhaseB = gpio_get_level(FASE_B1);

            if (PhaseB != prevPhaseB && PhaseB == 1) {
                delta = (PhaseA != PhaseB) ? 1 : -1;
                set_direction(&encoder2, delta > 0 ? CW : CCW);
            }
            set_phaseA(&encoder2, PhaseA);
            set_phaseB(&encoder2, PhaseB);

            // Update position based on delta
            currentPosition = get_position(&encoder2) + delta;
            set_position(&encoder2, currentPosition);

            // Check for position limits
            int32_t stepsPerRevolution = get_stepsPerRevolution(&encoder2);
            if (currentPosition >= stepsPerRevolution || currentPosition <= -stepsPerRevolution) {
                set_position(&encoder2, 0);
            }
            prevPhaseA = PhaseA;
            prevPhaseB = PhaseB;
            counter += abs(delta);
            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////update speed////////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            // Calculate speed by counting the pulses during 1 milli second
            if (currentTime - prevTime >= 1e3){
                // Calculate position change
                speed = (((counter*1e6)/(currentTime - prevTime))*60)/get_stepsPerRevolution(&encoder2); //((step/s)/(steps/rev))*(60s/1min)=rpm
                set_speed(&encoder2, speed);
                
                // Measure time using a hardware timer
                prevTime = currentTime;
                // Reboot step counter
                counter = 0;
                //////////////////////////////////////////////////////////////////////////
                /////////////////////////////Log Message//////////////////////////////////
                xSemaphoreGive(encoder2Semaphore);
                xQueueSend(motor2SpeedQueue, &speed, 0);
                //////////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////
            }
            else{
                xSemaphoreGive(encoder2Semaphore);  
            }
        }
    }
}

void DirectKinematicsTask(void *pvParameters) {
    DirectKinematicsParams_t* params = (DirectKinematicsParams_t*)pvParameters;
    QueueHandle_t motor1SpeedQueue = params->motor1SpeedQueue;
    QueueHandle_t motor2SpeedQueue = params->motor2SpeedQueue;

    float motor1_speed = 0.0;
    float motor2_speed = 0.0;

    while (1) {
        // Read the speed values from the motor1SpeedQueue and motor2SpeedQueue
        if (xQueueReceive(motor1SpeedQueue, &motor1_speed, portMAX_DELAY) == pdTRUE &&
            xQueueReceive(motor2SpeedQueue, &motor2_speed, portMAX_DELAY) == pdTRUE) {
            // Perform direct kinematics calculations here using motor1_speed and motor2_speed
            
            float robot_speed = (motor1_speed + motor2_speed)/2; //1=left 2=right
            float robot_angle = (motor2_speed - motor1_speed)/(2*ROBOT_WIDTH); 

            // Do whatever processing you need with the direct kinematics results

            // Log or send the results as needed
            ESP_LOGI("Direct Kinematics:", "Lineal Speed: %f Angular Speed: %f", robot_speed, robot_angle);
        }

        // Delay for task execution rate control
        vTaskDelay(pdMS_TO_TICKS(10)); // 10 ms delay
    }
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
