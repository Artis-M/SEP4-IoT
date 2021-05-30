/*
/*
 * CO2Handler.c
 *
 * Created: 4/25/2021 5:17:54 PM
 *  Author: 
 */ 
#include <mh_z19.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include "CO2Handler.h"
#include "SharedPrint.h"

#include "event_groups.h"

uint16_t ppm;
mh_z19_returnCode_t rc;

void myCo2CallBack(uint16_t ppmCall);
void start_co2_task(void* self);

EventGroupHandle_t task_startGroup;
EventBits_t _readyBit;

typedef struct CO2Handler
{
	uint16_t co2ppm;
	int co2MeasurementCount;
	int16_t averageCO2;
	
} CO2Handler;

CO2Handler_t co2_create(UBaseType_t co2_task_priority, EventGroupHandle_t eventBits, EventBits_t bits){
	CO2Handler_t _new_reader = calloc(1, sizeof(CO2Handler));
	if (_new_reader == NULL){
		return NULL;
	}
	_new_reader->co2ppm = 0;
	_new_reader->averageCO2 = 0;
	_new_reader->co2MeasurementCount = 0;
	initialiseCO2Sensor();
	
	_readyBit = bits;
	task_startGroup = eventBits;
	
	rc = mh_z19_takeMeassuring();
	co2_initialize_task(co2_task_priority, _new_reader);
	return _new_reader;
}

void start_co2_task(void* self){
		TickType_t xLastWakeTime;
		const TickType_t xFrequency = pdMS_TO_TICKS(16000UL);
		xLastWakeTime = xTaskGetTickCount();
		for(;;)
		{
			xTaskDelayUntil( &xLastWakeTime, xFrequency );
			co2_handler_task((CO2Handler_t) self);
	}
}

void reset_averageCO2(CO2Handler_t self){
	self->co2MeasurementCount = 0;
	self->averageCO2 = 0;
}

void co2_initialize_task(UBaseType_t co2_task_priority, CO2Handler_t self)
{
	
	xEventGroupSetBits(task_startGroup, _readyBit);
	
	xTaskCreate(
	start_co2_task
	,  "co2Task"
	,  configMINIMAL_STACK_SIZE+100
	,  (void*) self
	,  co2_task_priority
	,  NULL );
}

void initialiseCO2Sensor(){
	mh_z19_initialise(ser_USART3); 
	mh_z19_injectCallBack(myCo2CallBack);
}

void myCo2CallBack(uint16_t ppmCall)
{
	ppm = ppmCall;
}

uint16_t getCO2(CO2Handler_t self){
	self->co2ppm = self->averageCO2 / self->co2MeasurementCount;
	return self->co2ppm;
}

void co2_handler_task(CO2Handler_t self){


	EventBits_t readyBits = xEventGroupWaitBits(task_startGroup,
	_readyBit,
	pdFALSE,
	pdTRUE,
	portMAX_DELAY);
		
		if ((readyBits & (_readyBit)) == (_readyBit)) {
			rc = mh_z19_takeMeassuring();
			if (rc != MHZ19_OK)
			{
				printShared("CO2 Measurement failed.");
				return;
			}
			self->co2MeasurementCount++;
			self->averageCO2 += ppm;
			printShared("Measurement number: %d \n", self->co2MeasurementCount);
			printShared("Average CO2: %d \n", self->averageCO2 / self->co2MeasurementCount);
		}	
}