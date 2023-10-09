/*******************************************************************************
 * @file        main.c
 * @author      Leonardo Acha Boiano
 * @date        12 August 2023
 * @brief       Main code, low level controller brushed motor
 * 
 * @note        This code is written in C and is used on the ESP32 DEVKIT V1 board.
 *
 *******************************************************************************/

#include "main.h"

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    // Initialize NVS (Non-Volatile Storage)
    nvs_flash_init_custom(ret);

    // Connect to WiFi
    connect_wifi();

    // Start the web server
    start_webserver();

    //Initialize encoder
    init_encoder();

    // Initialize GPIO pins
    init_gpio();

    //Init timer
    init_timers();

    //Init interrupt gpios
    init_isr();

    //Init motor structures
    initialize_motors();

    //Initialize FreeRTOS Tasks
    initialize_tasks();
}

esp_err_t nvs_flash_init_custom(esp_err_t ret){
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    return ESP_OK;
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
