/*******************************************************************************
 * @file        encoder.h
 * @author      Leonardo Acha Boiano
 * @date        25 Aug 2023
 * 
 * @note        This code is written in C and is used on an ESP32 DEV KIT V1 board.
 *
 *******************************************************************************/
#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <string.h>
#include <math.h>

extern uint8_t EncoderStateA;
extern uint8_t EncoderStateB;

#define PPR 512
#define CW 0
#define CCW 1

// Define the encoder structure
typedef struct{
    volatile uint8_t phaseA;
    volatile uint8_t phaseB;
    uint16_t stepsPerRevolution;
    float degreesPerStep;
    volatile int16_t position[2];
    volatile float positionDegrees[2];
    volatile float speed[2];
    volatile uint8_t direction;
}Encoder;

// Declare encoder-related functions
void set_phaseA(Encoder *encoder, uint8_t phaseA);
uint8_t get_phaseA(const Encoder *encoder);

void set_phaseB(Encoder *encoder, uint8_t phaseB);
uint8_t get_phaseB(const Encoder *encoder);

uint16_t get_stepsPerRevolution(const Encoder *encoder);

float get_degreesPerStep(const Encoder *encoder);

void set_position(Encoder *encoder, int16_t position);
int16_t get_position(const Encoder *encoder);

float get_positionDegrees(const Encoder *encoder);

float get_previousPositionDegrees(const Encoder *encoder);

void set_speed(Encoder *encoder, float speed);
float get_speed(const Encoder *encoder);

void set_direction(Encoder *encoder, uint8_t new_direction);
uint8_t get_direction(const Encoder *encoder);

void init_encoder(Encoder *encoder);

double roundToDecimal(double num, uint8_t decimalPlaces);

#endif // ENCODER_H

/********************************* END OF FILE ********************************/
/******************************************************************************/
