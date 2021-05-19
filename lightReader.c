/*
 * lightReader.c
 *
 * Created: 25/04/2021 16:06:32
 *  Author: Cata
 */ 


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <ATMEGA_FreeRTOS.h>
#include <tsl2591.h>
#include "event_groups.h"
#include "task.h"
#include "lightReader.h"

int lightMeasurementCount = 1;
uint16_t averageLight;
void light_task_start(void* self);
void tsl2591Callback(tsl2591_returnCode_t rc, lightReader_t self);

typedef struct lightReader 
{
	uint16_t lux;
}lightReader;

lightReader_t initialiseLightDriver(){
	lightReader_t new_reader = calloc(1, sizeof(lightReader));
	
	if(new_reader == NULL) return NULL;
	
	new_reader->lux = 0;
	
	if ( TSL2591_OK == tsl2591_initialise(tsl2591Callback))
	{
		puts("Light driver initialized");
		printf("Return code is: %s", tsl2591_initialise(tsl2591Callback));
	}
	
	return new_reader;
}

void light_executeTask(lightReader_t self)
{
	for (;;)
	{
		getLightMeasurements(self);
	}
}

void light_initializeTask(UBaseType_t lightPriority, lightReader_t self)
{
	xTaskCreate(
	light_task_start
	,  "lightTask"
	,  configMINIMAL_STACK_SIZE+100
	,  (void*) self
	,  lightPriority
	,  NULL
	);
}

void tsl2591Callback(tsl2591_returnCode_t rc, lightReader_t self)
{
	uint16_t _tmp;
	float _lux;
	switch (rc)
	{
		case TSL2591_DATA_READY:
		if ( TSL2591_OK == (rc = tsl2591_getFullSpectrumRaw(&_tmp)) )
		{
			//printf("\nFull Raw:%04X\n", _tmp);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("\nFull spectrum overflow - change gain and integration time\n");
		}
		
		if ( TSL2591_OK == (rc = tsl259_getVisibleRaw(&_tmp)) )
		{
			//printf("Visible Raw:%04X\n", _tmp);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("Visible overflow - change gain and integration time\n");
		}
		
		if ( TSL2591_OK == (rc = tsl2591_getInfraredRaw(&_tmp)) )
		{
			//printf("Infrared Raw:%04X\n", _tmp);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("Infrared overflow - change gain and integration time\n");
		}
		
		if ( TSL2591_OK == (rc = tsl2591_getLux(&_lux)) )
		{
			//printf("Lux: %5.4f\n", _lux);
			self->lux = _lux;
			
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("Lux overflow - change gain and integration time\n");
		}
		break;
		
		case TSL2591_OK:
		// Last command performed successful
		break;
		
		case TSL2591_DEV_ID_READY:
		// Dev ID now fetched
		break;
		
		default:
		break;
	}
}

void getLightMeasurements(lightReader_t self){
	
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500/portTICK_PERIOD_MS; // 500 ms
	
	if ( TSL2591_OK == tsl2591_enable() )
	{
		puts("Light enabled");
		
		//somewhere here we MIGHT need to do an else and call callback with TSL_OK
		// The power up command is now send to the sensor - it can be powered down with a call to tsl2591_disable()
	}
	
	xTaskDelayUntil( &xLastWakeTime, xFrequency );
	
	if ( TSL2591_OK != tsl2591_fetchData() )
	{
		puts("Light not fetched");
		// Something went wrong
		// Investigate the return code further
	}
	else
	{
		tsl2591Callback(TSL2591_DATA_READY, self);
		
		//tsl2591_getLux(&light); //sth wrong here. Repair later
		//printf("The Light Data Received from the sensor is : %2.2f \n", light);
	}
}

uint16_t getLight(lightReader_t self){
	return self->lux;
}
void light_task_start(void* self){
		TickType_t xLastWakeTime;
		const TickType_t xFrequency = pdMS_TO_TICKS(15000UL);
		xLastWakeTime = xTaskGetTickCount();
		
		
	for(;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		light_handler_task((lightReader_t) self);
	}
}

void light_handler_task(lightReader_t self)
{
		lightMeasurementCount++;
		getLightMeasurements(self);
		if(lightMeasurementCount <= 20)
		{
			averageLight +=self->lux;
		}
		else
		{
			lightMeasurementCount = 1;
			averageLight = self->lux;
		}
		
		printf("Value of light: %d \n", averageLight/lightMeasurementCount);
		printf("Measurement number of light: %d \n", lightMeasurementCount);

}

