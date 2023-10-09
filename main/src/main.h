/*******************************************************************************
 * @file        main.h
 * @author      Leonardo Acha Boiano
 * @date        12 August 2023
 * @brief       Main code, low level controller brushed motor
 * 
 * @note        This code is written in C and is used on the ESP32 DEVKIT V1 board.
 *
 *******************************************************************************/

#ifndef MAIN_H
#define MAIN_H

#include "stdio.h"
#include "config.h"

#include <esp_system.h>
#include <nvs_flash.h>

#include "connect_wifi/connect_wifi.h"
#include "gpio_utils/gpio_utils.h"
#include "motor_control/motor_control.h"
#include "web_server/web_server.c"
#include "http_handlers/http_handlers.h"
#include "motor_control/motor_control.h"
#include "task_utils/task_utils.h"
#include "logging/logging_utils.h"
#include "encoder/encoder.h"

// Function prototypes
esp_err_t nvs_flash_init_custom(esp_err_t ret);

extern Encoder encoder1;
extern Encoder encoder2;

#endif /* MAIN_H */

/********************************* END OF FILE ********************************/
/******************************************************************************/
