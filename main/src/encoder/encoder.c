/*******************************************************************************
 * @file        encoder.c
 * @author      Leonardo Acha Boiano
 * @date        25 Aug 2023
 * 
 * @note        This code is written in C and is used on an ESP32 DEV KIT V1 board.
 *
 *******************************************************************************/
#include "../encoder/encoder.h"

Encoder create_encoder(uint8_t phaseA, uint8_t phaseB, uint16_t stepsPerRevolution) {
    Encoder encoder;
    encoder.phaseA = phaseA;
    encoder.phaseB = phaseB;
    encoder.stepsPerRevolution = stepsPerRevolution;
    encoder.degreesPerStep = 360.0 / stepsPerRevolution;
    encoder.direction= 2; // Default direction as 2
    // Initialize arrays and strings to zero or empty values
    memset(encoder.position, 0, sizeof(encoder.position));
    memset(encoder.positionDegrees, 0, sizeof(encoder.positionDegrees));
    memset(encoder.speed, 0, sizeof(encoder.speed));
    return encoder;
}

// Setter for phaseA
void set_phaseA(Encoder *encoder, uint8_t phaseA) {
    encoder->phaseA = phaseA;
}

// Getter for phaseA
uint8_t get_phaseA(const Encoder *encoder) {
    return encoder->phaseA;
}

// Setter for phaseB
void set_phaseB(Encoder *encoder, uint8_t phaseB) {
    encoder->phaseB = phaseB;
}

// Getter for phaseB
uint8_t get_phaseB(const Encoder *encoder) {
    return encoder->phaseB;
}

// Getter for stepsPerRevolution
uint16_t get_stepsPerRevolution(const Encoder *encoder) {
    return encoder->stepsPerRevolution;
}

// Getter for degressPerStep
float get_degreesPerStep(const Encoder *encoder) {
    return encoder->degreesPerStep;
}

// Setter for position
void set_position(Encoder *encoder, int16_t position) {
    //save  position as previous position
    encoder->positionDegrees[1]=encoder->positionDegrees[0];
    encoder->position[1]=encoder->position[0];  
    //update new position
    encoder->positionDegrees[0] = (encoder->degreesPerStep)*position;
    encoder->position[0] = position;
}

// Getter for position
int16_t get_position(const Encoder *encoder) {
    return encoder->position[0];
}

// Getter for positionDegress
float get_positionDegrees(const Encoder *encoder){
    return encoder->positionDegrees[0];
}

// Getter for previous position in degress
float get_previousPositionDegrees(const Encoder *encoder){
    return encoder->positionDegrees[1];
}

// Setter for speed
void set_speed(Encoder *encoder, int32_t speed){
    encoder->speed[1]=encoder->speed[0];
    encoder->speed[0] = speed;
}

// Getter for speed
int32_t get_speed(const Encoder *encoder) {
    return encoder->speed[0];
}

// Setter for direction
void set_direction(Encoder *encoder, uint8_t new_direction) {
    encoder->direction = new_direction;
}

// Getter for direction
uint8_t get_direction(const Encoder *encoder) {
    return encoder->direction;
}

//Global encoder structure
Encoder encoder1;

void init_encoder(Encoder *encoder) {
    // Initialize encoders
    encoder1 = create_encoder(0, 0, PPR);
}

double roundToDecimal(double num, uint8_t decimalPlaces) {
    double multiplier = pow(10, decimalPlaces);
    return round(num * multiplier) / multiplier;
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
