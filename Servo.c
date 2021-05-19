/*
 * Servo.c
 *
 * Created: 12/05/2021 12:06:30
 *  Author: Cata
 */ 

#include "Servo.h"
#include <avr/io.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <rc_servo.h>
#include <stdio.h>

typedef struct Servo
{
	uint8_t servoNo;
	int8_t 	percent;
}servo;

servo_t servo_initialise()
{
	servo_t newServo = calloc(1, sizeof(servo));
	
	if(newServo == NULL) return NULL;
	
	newServo->servoNo = 0; //J14
	newServo->percent = 0; //middle position
	
	rc_servo_initialise();
	printf("Servo initialized");
	
	return newServo;
}

void servo_setPosition(uint8_t servoNo, int8_t percent)
{
	if(servoNo != 0)
	{ 
		printf("Only Servo#0 can be accessed");
	}
	else if (percent == NULL)
	{
		printf("No input from user");
	}
	else 
	{
		//servo_t->percent = percent; what is going on I am dying - Artis
	}
}


