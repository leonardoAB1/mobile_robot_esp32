/*******************************************************************************
 * @file        gpio_utils.c
 * @author      Leonardo Acha Boiano
 * @date        11 Jun 2023
 * 
 * @note        This code is written in C and is used on an ESP32 DEV KIT V1 board.
 *
 *******************************************************************************/
#include "../gpio_utils/gpio_utils.h"

TimerHandle_t xTimer1;
TimerHandle_t xTimer2;
uint8_t timerID = 1;

esp_err_t init_gpio(void) {
    // Initialize GPIO pins for servos as PWM outputs
    // Motor1
    ledc_channel_config_t channelConfigMotor1 = {0};
    channelConfigMotor1.gpio_num     = GPIO_ENA;
    channelConfigMotor1.speed_mode   = MOTOR_1_SPEED_MODE;
    channelConfigMotor1.channel      = MOTOR_1_CHANNEL;
    channelConfigMotor1.intr_type    = LEDC_INTR_DISABLE;
    channelConfigMotor1.timer_sel    = LEDC_TIMER_1;
    channelConfigMotor1.duty         = 0; //Initial Duty Cycle
    ledc_channel_config(&channelConfigMotor1);

    // Motor2
    ledc_channel_config_t channelConfigMotor2 = {0};
    channelConfigMotor2.gpio_num     = GPIO_ENB;
    channelConfigMotor2.speed_mode   = MOTOR_2_SPEED_MODE;
    channelConfigMotor2.channel      = MOTOR_2_CHANNEL;
    channelConfigMotor2.intr_type    = LEDC_INTR_DISABLE;
    channelConfigMotor2.timer_sel    = LEDC_TIMER_1;
    channelConfigMotor2.duty         = 0; //Initial Duty Cycle
    ledc_channel_config(&channelConfigMotor2);

    //Initialize Timer
    ledc_timer_config_t timerConfig = {0};
    timerConfig.speed_mode          = LEDC_HIGH_SPEED_MODE;
    timerConfig.duty_resolution     = PWM_RESOLUTION; //10 bit
    timerConfig.timer_num           = LEDC_TIMER_1;
    timerConfig.freq_hz             = 50; //50 Hz
    ledc_timer_config(&timerConfig);

    // Initialize normal GPIO pins as digital output
    gpio_reset_pin(GPIO_IN1); //IN1
    gpio_set_direction(GPIO_IN1, GPIO_MODE_OUTPUT);

    gpio_reset_pin(GPIO_IN2); //IN2
    gpio_set_direction(GPIO_IN2, GPIO_MODE_OUTPUT);

    gpio_reset_pin(GPIO_IN3); //IN3
    gpio_set_direction(GPIO_IN3, GPIO_MODE_OUTPUT);

    gpio_reset_pin(GPIO_IN4); //IN4
    gpio_set_direction(GPIO_IN4, GPIO_MODE_OUTPUT);

    //Initize encoder values
    set_phaseA(&encoder1, gpio_get_level(FASE_A1));
    set_phaseB(&encoder1, gpio_get_level(FASE_B1));
    set_phaseA(&encoder2, gpio_get_level(FASE_A2));
    set_phaseB(&encoder2, gpio_get_level(FASE_B2));
   
    return ESP_OK;
}

esp_err_t init_timers(void){
    xTimer1 = xTimerCreate("Timer1",       // Just a text name, not used by the kernel.
                        pdMS_TO_TICKS(1000),   // The timer period in ticks.
                        pdTRUE,        // The timers will auto-reload themselves when they expire.
                        ( void * ) timerID,  // Assign each timer a unique id equal to its array index.
                        vTimer1Callback // Each timer calls the same callback when it expires.
                        );

    if( xTimer1 == NULL )
    {
        // The timer was not created.
        ESP_LOGE(GPIO_TAG, "Timer 1 was not created.");
    }
    else
    {
        // Start the timer.  No block time is specified, and even if one was
        // it would be ignored because the scheduler has not yet been
        // started.
        if( xTimerStart( xTimer1, 0 ) != pdPASS )
        {
            // The timer could not be set into the Active state.
            ESP_LOGE(GPIO_TAG, "Timer 1 could not be set into the Active state.");
        }
    }

    xTimer2 = xTimerCreate("Timer2",       // Just a text name, not used by the kernel.
                        pdMS_TO_TICKS(1000),   // The timer period in ticks.
                        pdTRUE,        // The timers will auto-reload themselves when they expire.
                        ( void * ) timerID,  // Assign each timer a unique id equal to its array index.
                        vTimer2Callback // Each timer calls the same callback when it expires.
                        );

    if( xTimer2 == NULL )
    {
        // The timer was not created.
        ESP_LOGE(GPIO_TAG, "Timer 2 was not created.");
    }
    else
    {
        // Start the timer.  No block time is specified, and even if one was
        // it would be ignored because the scheduler has not yet been
        // started.
        if( xTimerStart( xTimer2, 0 ) != pdPASS )
        {
            // The timer could not be set into the Active state.
            ESP_LOGE(GPIO_TAG, "Timer 2 could not be set into the Active state.");
        }
    }

    return ESP_OK;
}

void vTimer1Callback( TimerHandle_t pxTimer){
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(encoder1Binary, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

void vTimer2Callback( TimerHandle_t pxTimer){
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(encoder2Binary, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t init_isr(void)
{   
    // Define the interrupt initialization parameters
    InterruptInitParams_t const InterruptInitParameters[] = {
        {FASE_A1, update_encoder1_isr, NULL, GPIO_INTR_ANYEDGE},
        {FASE_B1, update_encoder1_isr, NULL, GPIO_INTR_ANYEDGE},
        {FASE_A2, update_encoder2_isr, NULL, GPIO_INTR_ANYEDGE},
        {FASE_B2, update_encoder2_isr, NULL, GPIO_INTR_ANYEDGE}
        // Add more interrupts here if needed
    };

    for (size_t i = 0; i < sizeof(InterruptInitParameters) / sizeof(InterruptInitParameters[0]); i++)
    {
        gpio_config_t gpioConfig;
        gpioConfig.pin_bit_mask = (1ULL << InterruptInitParameters[i].gpioNum);
        gpioConfig.mode = GPIO_MODE_DEF_INPUT;
        gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
        gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
        gpioConfig.intr_type = InterruptInitParameters[i].interruptType;

        esp_err_t err = gpio_config(&gpioConfig);
        if (err != ESP_OK) {
            ESP_LOGE(INTERRUPT_LOG_TAG, "GPIO config failed for pin %d with error 0x%x", InterruptInitParameters[i].gpioNum, err);
            return err;
        }

        if (i==0){
            gpio_install_isr_service(0);
        }

        err = gpio_isr_handler_add(InterruptInitParameters[i].gpioNum, InterruptInitParameters[i].isrHandler, InterruptInitParameters[i].userData);
        if (err != ESP_OK) {
            ESP_LOGE(INTERRUPT_LOG_TAG, "GPIO ISR handler add failed for pin %d with error 0x%x", InterruptInitParameters[i].gpioNum, err);
            return err;
        }
    }

    ESP_LOGI(INTERRUPT_LOG_TAG, "Interrupts Initialized");
    return ESP_OK;
}

// ISR for encoder1
void IRAM_ATTR update_encoder1_isr(void* arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(encoder1Binary, &xHigherPriorityTaskWoken);

    // Reset the timer
    xTimerReset(xTimer1, 0);

    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

// ISR for encoder2
void IRAM_ATTR update_encoder2_isr(void* arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(encoder2Binary, &xHigherPriorityTaskWoken);

    // Reset the timer
    xTimerReset(xTimer2, 0);

    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
