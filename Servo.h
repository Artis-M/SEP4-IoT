/*
 * Servo.h
 *
 * Created: 12/05/2021 12:05:57
 *  Author: Cata
 */ 
#pragma once

#include <ATMEGA_FreeRTOS.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "rc_servo.h"

typedef struct Servo* servo_t;

servo_t servo_initialise(void);
void servo_setPosition(uint8_t 	servoNo,int8_t 	percent);