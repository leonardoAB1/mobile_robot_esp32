/*******************************************************************************
 * @file        web_server.c
 * @brief       Web Server Implementation
 * @details     This file contains the implementation of a web server for an ESP32-CAM development board.
 *              It provides functions to start the web server and register URI handlers.
 * @author      Leonardo Acha Boiano
 * @date        7 Jun 2023
 *
 * @note        This code is written in C and is used on an ESP32-CAM development board.
 *
 *******************************************************************************/

#include "../web_server/web_server.h"

// Function to start the web server
void start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.core_id = 1;  // Set the core ID to 0 (core 0)
    config.max_uri_handlers = 20;
    config.task_priority    = tskIDLE_PRIORITY+1;
    //config.uri_match_fn     = httpd_uri_match_wildcard;

    // Start the HTTP server
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        httpd_uri_t status_uri = {
            .uri = "/status",
            .method = HTTP_GET,
            .handler = status_httpd_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &status_uri);

        httpd_uri_t reference_set_uri = {
            .uri = "/reference/set",
            .method = HTTP_POST,
            .handler = handle_set_reference, 
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &reference_set_uri);

        httpd_uri_t reference_get_uri = {
            .uri = "/reference/get",
            .method = HTTP_GET,
            .handler = handle_get_reference, 
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &reference_get_uri);

        httpd_uri_t encoder_get_uri = {
            .uri = "/encoder/get",
            .method = HTTP_GET,
            .handler = handle_get_encoder, 
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &encoder_get_uri);

        ESP_LOGI(WEBSERVER_TAG, "HTTP server started");
    }
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
