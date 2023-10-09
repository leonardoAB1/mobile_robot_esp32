/*******************************************************************************
 * @file        http_handlers.h
 * @brief       Header file containing declarations of HTTP request handlers.
 * @author      Leonardo Acha Boiano
 * @date        7 Jun 2023
 * 
 * @note        This code is written in C and is used on an ESP32-CAM development board.
 *
 *******************************************************************************/

#ifndef HTTP_HANDLERS_H
#define HTTP_HANDLERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include <cJSON.h>
#include <esp_http_server.h>

#include "../logging/logging_utils.h"
#include "../gpio_state/gpio_state.h"
#include "../motor_control/motor_control.h"
#include "../encoder/encoder.h"

#define ROBOT_WIDTH 2*97*1e-3
#define WHEEL_DIAMETER 66.42*1e-3

/**
 * @brief       HTTP request handler for getting the camera status.
 * @param[in]   req The HTTP request object.
 * @return      An esp_err_t indicating the success or failure of the operation.
 */
esp_err_t status_httpd_handler(httpd_req_t *req);

esp_err_t handle_set_reference1(httpd_req_t *req);

esp_err_t handle_get_reference1(httpd_req_t *req);

esp_err_t handle_get_encoder1(httpd_req_t *req);

esp_err_t handle_set_reference2(httpd_req_t *req);

esp_err_t handle_get_reference2(httpd_req_t *req);

esp_err_t handle_get_encoder2(httpd_req_t *req);

esp_err_t handle_set_robot_speed(httpd_req_t *req);

extern Motor motor1;
extern Motor motor2;
extern Encoder encoder1;
extern Encoder encoder2;
extern uint8_t ControlStrategy;

#endif  // HTTP_HANDLERS_H

/********************************* END OF FILE ********************************/
/******************************************************************************/
