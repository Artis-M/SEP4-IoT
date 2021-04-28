
/*
 * TemperatureHandler.c
 *
 * Created: 4/25/2021 3:55:16 PM
 *  Author: Artis
 */ 

#include <avr/io.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hih8120.h>
#include <stdio.h>
#include "TemperatureHandler.h"

typedef struct temperatureHandler{
	int16_t temperature;
	uint16_t humidity;
} temperatureHandler;

temperatureHandler_t temperatureHandler_create(){
	temperatureHandler_t _new_reader = calloc(1, sizeof(temperatureHandler));
		if (_new_reader == NULL){
				return NULL;
		}
	_new_reader->temperature = 0;
	_new_reader->humidity = 0;
	
	if ( HIH8120_OK == hih8120_initialise() )
	{
		printf("Temp sensor initialized");
		printf("_______________________");
	}
	printf("Temperature sensor init?");
	return _new_reader;
}
void getTemperatureMesurements(temperatureHandler_t self){
	
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500/portTICK_PERIOD_MS; // 500 ms
	
	if ( HIH8120_OK != hih8120_wakeup() )
	{
		// Something went wrong
		// Investigate the return code further
		puts("Task1 failed to work!");
	}
	
	xTaskDelayUntil( &xLastWakeTime, xFrequency );

	
	if ( HIH8120_OK !=  hih8120_measure() )
	{
		// Something went wrong
		// Investigate the return code further
		puts("Task1 failed to work again");
	}
	
	self->temperature = hih8120_getTemperature();
	self->humidity = hih8120_getHumidity();
	
}
int16_t getTemperature(temperatureHandler_t self){
		printf("%d", self->temperature);
			return self->temperature;
		
}
uint16_t getHumidity(temperatureHandler_t self){
	printf("%d", self->humidity);
	return self->humidity;
	
}