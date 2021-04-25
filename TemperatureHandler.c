
/*
 * TemperatureHandler.c
 *
 * Created: 4/25/2021 3:55:16 PM
 *  Author: Artis
 */ 
#include <hih8120.h>
#include <stdio.h>

void initialiseTempDriver(){
	if ( HIH8120_OK == hih8120_initialise() )
	{
		printf("Temp sensor initialized");
		printf("_______________________");
	}
}
void getMeasurements(){
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
}
float getTemperature(){
			return hih8120_getTemperature();
}
float getHumidity(){
	return hih8120_getHumidity();
}