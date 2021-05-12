
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

	int measurementCount = 1;
	int16_t averageTemp;
	uint16_t averageHumidity;
	
typedef struct temperatureHandler{
	int16_t temperature;
	uint16_t humidity;
} temperatureHandler;

void temperature_handler_task( void *pvParameters );

void temperature_handler_initialise(UBaseType_t temperatureHandler_priority)
{
	printf("SETTING UP LORAWAN");
	xTaskCreate(
	temperature_handler_task
	,  "TemperatureTask"
	,  configMINIMAL_STACK_SIZE+100
	,  NULL
	,  temperatureHandler_priority
	,  NULL );
}





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
	
	temperature_handler_initialise(3);
	return _new_reader;
}
void getTemperatureMesurements(temperatureHandler_t self){
	
	//TickType_t xLastWakeTime;
	//const TickType_t xFrequency = 200/portTICK_PERIOD_MS; // 200 ms
	//xTaskDelayUntil( &xLastWakeTime, xFrequency );
	//self->temperature = hih8120_getTemperature();
	//xTaskDelayUntil( &xLastWakeTime, xFrequency );
	//self->humidity = hih8120_getHumidity();
	
		self->temperature = averageTemp / measurementCount;
		self->humidity = averageHumidity / measurementCount;
		
	printf("Getting temperature measurements from function.");
	
}
int16_t getTemperature(temperatureHandler_t self){
		printf("Temperature: %d", self->temperature);
			return self->temperature;
		
}
uint16_t getHumidity(temperatureHandler_t self){
	printf("Humidity: %d", self->humidity);
	return self->humidity;
	
}


void temperature_handler_task( void *pvParameters ){
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(15000UL);
	const TickType_t xFrequency2 = pdMS_TO_TICKS(100UL);
	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		
		if ( HIH8120_OK != hih8120_wakeup() )
		{
			// Something went wrong
			// Investigate the return code further
			puts("Temp task failed to work!");
		}
		
			xTaskDelayUntil( &xLastWakeTime, xFrequency2 );
			if ( HIH8120_OK !=  hih8120_measure() )
			{
				// Something went wrong
				// Investigate the return code further
				puts("Temp task failed to work again!");
			}
			
		measurementCount++;
		if(measurementCount <= 20){
			averageTemp += hih8120_getTemperature();
			averageHumidity += hih8120_getHumidity();
		}
		else{
			measurementCount = 1;
				averageTemp = hih8120_getTemperature();
				averageHumidity = hih8120_getHumidity();
		}
		printf("Measurement number: %d", measurementCount);
		printf("Got temperature measurements");
	}
}
