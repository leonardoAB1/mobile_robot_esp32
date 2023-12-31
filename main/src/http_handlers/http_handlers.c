/*******************************************************************************
 * @file        http_handlers.c
 * @author      Leonardo Acha Boiano
 * @date        7 Jun 2023
 * 
 * @note        This code is written in C and is used ESP32 BEVKIT V1 board.
 *
 *******************************************************************************/

#include "../http_handlers/http_handlers.h"

// HTTP request handler for getting the camera status
esp_err_t status_httpd_handler(httpd_req_t *req)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "status", "online");

    char *response = cJSON_Print(root);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, strlen(response));

    cJSON_Delete(root);
    free(response);

    return ESP_OK;
}

esp_err_t handle_set_reference1(httpd_req_t *req)
{
    // Check if the request method is POST
    if (req->method != HTTP_POST) {
        httpd_resp_send_err(req, HTTPD_405_METHOD_NOT_ALLOWED, "Method Not Allowed");
        return ESP_OK;
    }
    // Extract the value of the controller header
    char controller[32];
    if (httpd_req_get_hdr_value_str(req, "Controller", controller, sizeof(controller)) == ESP_OK) {
        ControlStrategy = (strcmp(controller, "PID") == 0) ? PID_CONTROLLER : (strcmp(controller, "DEFAULT") == 0) ? OPEN_LOOP : INVALID_CONTROLLER;
    } else {
        // Handle the case where the "Controller" header is not present in the request
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
        return ESP_OK;
    }

    // Extract the value of the Adc-Resolution header
    char adcResolution[32];
    httpd_req_get_hdr_value_str(req, "Adc-Resolution", adcResolution, sizeof(adcResolution));
    // Convert char array to int
    uint8_t AdcResolution = atoi(adcResolution);

    // Get the content length
    char content_length_str[16];
    if (httpd_req_get_hdr_value_str(req, "Content-Length", content_length_str, sizeof(content_length_str)) != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
        return ESP_OK;
    }

    size_t content_len = atoi(content_length_str);

    // Read the request content data
    char *content = malloc(content_len + 1);
    if (!content) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Internal Server Error");
        return ESP_OK;
    }

    int ret = httpd_req_recv(req, content, content_len);
    if (ret <= 0) {
        free(content);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
        return ESP_OK;
    }

    content[ret] = '\0'; // Null-terminate the content

    // Parse the JSON content
    cJSON *json = cJSON_Parse(content);
    if (!json) {
        free(content);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_OK;
    }

    // Get the value from the JSON object
    cJSON *valueObj = cJSON_GetObjectItem(json, "referenceSignal");
    if (!valueObj || !cJSON_IsNumber(valueObj)) {
        cJSON_Delete(json);
        free(content);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_OK;
    }

    int32_t referenceSignal = valueObj->valueint;

    if (ControlStrategy==OPEN_LOOP){
        // Process the referenceSignal using the AdcResolution
        uint32_t maxOriginalValue = (1 << AdcResolution) - 1;       // Calculate the maximum value of the original range
        float ratio = 100.0 / maxOriginalValue;                     // Calculate the ratio for mapping

        float mappedValue = referenceSignal * ratio;                // Map the referenceSignal to the new range

        setReference1State(mappedValue);                             //set reference to float from 0 to 100

    } else if (ControlStrategy==PID_CONTROLLER){
        //Speed Saturation
        if (referenceSignal > 200) {
            referenceSignal = 200;
        }
        else if (referenceSignal < -200) {
            referenceSignal = -200;
        }
        setReference1State(referenceSignal); 
    }
    

    // Log the updated value
    ESP_LOGI(CONTROL_TAG, "Value updated: %.2f", getReference1State());  // Log the mapped value with two decimal places

    // Cleanup
    cJSON_Delete(json);
    free(content);

    // Send a response
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, "Value updated successfully", -1);

    return ESP_OK;
}

esp_err_t handle_get_reference1(httpd_req_t *req)
{
    uint8_t reference = getReference1State();
    // Create a JSON response payload with the reference state
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "reference_state", reference);
    char *payload = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);

    // Set the Content-Type header to application/json
    httpd_resp_set_type(req, "application/json");

    // Send the JSON response payload
    httpd_resp_send(req, payload, strlen(payload));

    // Free the allocated JSON payload
    free(payload);

    return ESP_OK;
}

esp_err_t handle_get_encoder1(httpd_req_t *req)
{
    // Create a JSON response payload with the encoder attributes
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "phaseA", get_phaseA(&encoder1));
    cJSON_AddNumberToObject(json, "phaseB", get_phaseB(&encoder1));
    cJSON_AddNumberToObject(json, "stepsPerRevolution", get_stepsPerRevolution(&encoder1));
    cJSON_AddNumberToObject(json, "degreesPerStep", get_degreesPerStep(&encoder1));
    cJSON_AddNumberToObject(json, "position", get_position(&encoder1));
    cJSON_AddNumberToObject(json, "positionDegrees", get_positionDegrees(&encoder1));
    cJSON_AddNumberToObject(json, "speed", get_speed(&encoder1));
    cJSON_AddNumberToObject(json, "direction", get_direction(&encoder1));
    char *payload = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);

    // Set the Content-Type header to application/json
    httpd_resp_set_type(req, "application/json");

    // Send the JSON response payload
    httpd_resp_send(req, payload, strlen(payload));

    // Free the allocated JSON payload if it's not NULL
    if (payload != NULL) {
        free(payload);
    }

    return ESP_OK;
}

esp_err_t handle_set_reference2(httpd_req_t *req)
{
    // Check if the request method is POST
    if (req->method != HTTP_POST) {
        httpd_resp_send_err(req, HTTPD_405_METHOD_NOT_ALLOWED, "Method Not Allowed");
        return ESP_OK;
    }
    // Extract the value of the controller header
    char controller[32];
    if (httpd_req_get_hdr_value_str(req, "Controller", controller, sizeof(controller)) == ESP_OK) {
        ControlStrategy = (strcmp(controller, "PID") == 0) ? PID_CONTROLLER : (strcmp(controller, "DEFAULT") == 0) ? OPEN_LOOP : INVALID_CONTROLLER;
    } else {
        // Handle the case where the "Controller" header is not present in the request
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
        return ESP_OK;
    }

    // Extract the value of the Adc-Resolution header
    char adcResolution[32];
    httpd_req_get_hdr_value_str(req, "Adc-Resolution", adcResolution, sizeof(adcResolution));
    // Convert char array to int
    uint8_t AdcResolution = atoi(adcResolution);

    // Get the content length
    char content_length_str[16];
    if (httpd_req_get_hdr_value_str(req, "Content-Length", content_length_str, sizeof(content_length_str)) != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
        return ESP_OK;
    }

    size_t content_len = atoi(content_length_str);

    // Read the request content data
    char *content = malloc(content_len + 1);
    if (!content) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Internal Server Error");
        return ESP_OK;
    }

    int ret = httpd_req_recv(req, content, content_len);
    if (ret <= 0) {
        free(content);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
        return ESP_OK;
    }

    content[ret] = '\0'; // Null-terminate the content

    // Parse the JSON content
    cJSON *json = cJSON_Parse(content);
    if (!json) {
        free(content);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_OK;
    }

    // Get the value from the JSON object
    cJSON *valueObj = cJSON_GetObjectItem(json, "referenceSignal");
    if (!valueObj || !cJSON_IsNumber(valueObj)) {
        cJSON_Delete(json);
        free(content);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_OK;
    }

    int32_t referenceSignal = valueObj->valueint;

    if (ControlStrategy==OPEN_LOOP){
        // Process the referenceSignal using the AdcResolution
        uint32_t maxOriginalValue = (1 << AdcResolution) - 1;       // Calculate the maximum value of the original range
        float ratio = 100.0 / maxOriginalValue;                     // Calculate the ratio for mapping

        float mappedValue = referenceSignal * ratio;                // Map the referenceSignal to the new range

        setReference2State(mappedValue);                             //set reference to float from 0 to 100

    } else if (ControlStrategy==PID_CONTROLLER){
        //Speed Saturation
        if (referenceSignal > 200) {
            referenceSignal = 200;
        }
        else if (referenceSignal < -200) {
            referenceSignal = -200;
        }
        setReference2State(referenceSignal); 
    }
    

    // Log the updated value
    ESP_LOGI(CONTROL_TAG, "Value updated: %.2f", getReference2State());  // Log the mapped value with two decimal places

    // Cleanup
    cJSON_Delete(json);
    free(content);

    // Send a response
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, "Value updated successfully", -1);

    return ESP_OK;
}

esp_err_t handle_get_reference2(httpd_req_t *req)
{
    uint8_t reference = getReference2State();
    // Create a JSON response payload with the reference state
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "reference_state", reference);
    char *payload = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);

    // Set the Content-Type header to application/json
    httpd_resp_set_type(req, "application/json");

    // Send the JSON response payload
    httpd_resp_send(req, payload, strlen(payload));

    // Free the allocated JSON payload
    free(payload);

    return ESP_OK;
}

esp_err_t handle_get_encoder2(httpd_req_t *req)
{
    // Create a JSON response payload with the encoder attributes
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "phaseA", get_phaseA(&encoder2));
    cJSON_AddNumberToObject(json, "phaseB", get_phaseB(&encoder2));
    cJSON_AddNumberToObject(json, "stepsPerRevolution", get_stepsPerRevolution(&encoder2));
    cJSON_AddNumberToObject(json, "degreesPerStep", get_degreesPerStep(&encoder2));
    cJSON_AddNumberToObject(json, "position", get_position(&encoder2));
    cJSON_AddNumberToObject(json, "positionDegrees", get_positionDegrees(&encoder2));
    cJSON_AddNumberToObject(json, "speed", get_speed(&encoder2));
    cJSON_AddNumberToObject(json, "direction", get_direction(&encoder2));
    char *payload = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);

    // Set the Content-Type header to application/json
    httpd_resp_set_type(req, "application/json");

    // Send the JSON response payload
    httpd_resp_send(req, payload, strlen(payload));

    // Free the allocated JSON payload if it's not NULL
    if (payload != NULL) {
        free(payload);
    }

    return ESP_OK;
}

esp_err_t handle_get_robot_speed(httpd_req_t *req)
{
    // Create a JSON response payload
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "robotSpeed", getSpeedState());
    cJSON_AddNumberToObject(json, "robotAngularSpeed", getAngularSpeedState());
    char *payload = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);

    // Set the Content-Type header to application/json
    httpd_resp_set_type(req, "application/json");

    // Send the JSON response payload
    httpd_resp_send(req, payload, strlen(payload));

    // Free the allocated JSON payload if it's not NULL
    if (payload != NULL) {
        free(payload);
    }

    return ESP_OK;
}

esp_err_t handle_set_robot_speed(httpd_req_t *req)
{
    // Check if the request method is POST
    if (req->method != HTTP_POST) {
        httpd_resp_send_err(req, HTTPD_405_METHOD_NOT_ALLOWED, "Method Not Allowed");
        return ESP_OK;
    }

    // Get the content length
    char content_length_str[16];
    if (httpd_req_get_hdr_value_str(req, "Content-Length", content_length_str, sizeof(content_length_str)) != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
        return ESP_OK;
    }

    size_t content_len = atoi(content_length_str);

    // Read the request content data
    char *content = malloc(content_len + 1);
    if (!content) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Internal Server Error");
        return ESP_OK;
    }

    int ret = httpd_req_recv(req, content, content_len);
    if (ret <= 0) {
        free(content);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
        return ESP_OK;
    }

    content[ret] = '\0'; // Null-terminate the content

    // Parse the JSON content
    cJSON *json = cJSON_Parse(content);
    if (!json) {
        free(content);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_OK;
    }

    // Get the values from the JSON object
    cJSON *speedObj = cJSON_GetObjectItem(json, "robot_speed");
    cJSON *angleObj = cJSON_GetObjectItem(json, "robot_angle");

    if (!speedObj || !angleObj || !cJSON_IsNumber(speedObj) || !cJSON_IsNumber(angleObj)) {
        cJSON_Delete(json);
        free(content);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_OK;
    }

    // Extract robot speed and angle from JSON
    float robot_speed = speedObj->valuedouble;//m/s
    float robot_angle = angleObj->valuedouble;//radians

    // Calculate left and right wheel speeds using inverse kinematic equations
    float v_left = (robot_speed - robot_angle * ROBOT_WIDTH) / (WHEEL_DIAMETER/2);//(rad/s)
    v_left= v_left*(60/(2*M_PI));//(RPM)
    float v_right =(robot_speed + robot_angle * ROBOT_WIDTH) / (WHEEL_DIAMETER/2);//(rad/s)
    v_right= v_right*(60/(2*M_PI));//(RPM)

    // Apply speed saturation to limit the wheel speeds to a maximum of 200 RPM
    //CLOSE LOOP SATURATION
    if (v_left > 200.0) {
        v_left = 200.0;
    } else if (v_left < -200.0) {
        v_left = -200.0;
    }

    if (v_right > 200.0) {
        v_right = 200.0;
    } else if (v_right < -200.0) {
        v_right = -200.0;
    }
    
    //OPEN LOOP SATURATION
    //v_left = (v_left/200)*100;
    //v_right =(v_right/200)*100;

    // Set the references for the left and right motors' speeds
    //ControlStrategy=OPEN_LOOP;
    ControlStrategy=PID_CONTROLLER;
    setReference1State(v_left);  
    setReference2State(v_right); 

    // Log the updated values
    ESP_LOGI(CONTROL_TAG, "Left Motor Speed: %.2f, Right Motor Speed: %.2f", v_left, v_right);

    // Cleanup
    cJSON_Delete(json);
    free(content);

    // Send a response
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, "Robot speed updated successfully", -1);

    return ESP_OK;
}
esp_err_t handle_reset_robot_distance(httpd_req_t *req) {
    // Reset the robot's X, Y, and theta states
    resetRobotXState();
    resetRobotYState();
    resetRobotThetaState();

    // Send a response to indicate that the reset was successful
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, "Robot distance reset successfully", -1);

    return ESP_OK;
}

esp_err_t handle_get_robot_distance(httpd_req_t *req) {
    // Get the robot's X, Y, and theta states
    float x = getRobotXState();
    float y = getRobotYState();
    float theta = getRobotThetaState();

    // Create a JSON response payload with the robot's distance information
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "robot_x", x);
    cJSON_AddNumberToObject(json, "robot_y", y);
    cJSON_AddNumberToObject(json, "robot_theta", theta);

    char *payload = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);

    // Set the Content-Type header to application/json
    httpd_resp_set_type(req, "application/json");

    // Send the JSON response payload
    httpd_resp_send(req, payload, strlen(payload));

    // Free the allocated JSON payload
    free(payload);

    return ESP_OK;
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
