
/*
 * TemperatureHandler.c
 *
 * Created: 4/25/2021 3:55:16 PM
 *  Author: Artis
 */ 
#include <hih8120.h>
#include <stdio.h>

typedef struct temperatureHandler{
	int16_t temperature;
	int16_t humidity;
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
	return _new_reader;
}
void getTemperatureMesurements(temperatureHandler_t self){
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
			return self->temperature;
}
int16_t getHumidity(temperatureHandler_t self){
	return self->humidity();
}