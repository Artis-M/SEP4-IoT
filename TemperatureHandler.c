#include <avr/io.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hih8120.h>
#include <stdio.h>
#include <rc_servo.h>
#include "TemperatureHandler.h"
#include "SharedPrint.h"
#include "event_groups.h"

EventGroupHandle_t task_startGroup;
EventBits_t _readyBit;

typedef struct temperatureHandler {
	int16_t temperature;
	uint16_t humidity;
	int tempMeasurementCount;
	int16_t averageTemp;
	uint16_t averageHumidity;
} temperatureHandler;

void start_temperature_task(void* self);

void temperature_handler_initialise(UBaseType_t temperatureHandler_priority, temperatureHandler_t self)
{

	xEventGroupSetBits(task_startGroup, _readyBit);

	xTaskCreate(
		start_temperature_task
		, "TemperatureTask"
		, configMINIMAL_STACK_SIZE + 100
		, (void*)self
		, temperatureHandler_priority
		, NULL);
}

void start_temperature_task(void* self) {
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(15000UL);
	xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency2 = pdMS_TO_TICKS(100UL);

	for (;;) {
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
		if (HIH8120_OK != hih8120_wakeup())
		{
			printShared("Temp task failed to work!");
		}
		xTaskDelayUntil(&xLastWakeTime, xFrequency2);
		temperature_handler_task((temperatureHandler_t)self);
	}
}

void reset_averageTemperature(temperatureHandler_t self) {
	self->tempMeasurementCount = 0;
	self->averageHumidity = 0;
	self->averageTemp = 0;
}

temperatureHandler_t temperatureHandler_create(UBaseType_t temp_task_priority, EventGroupHandle_t eventBits, EventBits_t bits) {
	temperatureHandler_t _new_reader = calloc(1, sizeof(temperatureHandler));
	if (_new_reader == NULL) {
		return NULL;
	}
	_new_reader->temperature = 0;
	_new_reader->humidity = 0;
	_new_reader->averageHumidity = 0;
	_new_reader->averageTemp = 0;
	_new_reader->tempMeasurementCount = 0;

	_readyBit = bits;
	task_startGroup = eventBits;

	if (HIH8120_OK == hih8120_initialise())
	{
		printShared("Temp sensor initialized \n");
	}

	temperature_handler_initialise(temp_task_priority, _new_reader);
	return _new_reader;
}
void getTemperatureMesurements(temperatureHandler_t self) {
	self->temperature = self->averageTemp / self->tempMeasurementCount;
	self->humidity = self->averageHumidity / self->tempMeasurementCount;
	printShared("Getting temperature measurements from function.");

}
int16_t getTemperature(temperatureHandler_t self) {
	printShared("Temperature: %d", self->temperature);
	return self->temperature;

}
uint16_t getHumidity(temperatureHandler_t self) {
	printShared("Humidity: %d", self->humidity);
	return self->humidity;

}


void temperature_handler_task(temperatureHandler_t self) {

	EventBits_t readyBits = xEventGroupWaitBits(task_startGroup,
		_readyBit,
		pdFALSE,
		pdTRUE,
		portMAX_DELAY);

	if ((readyBits & (_readyBit)) == (_readyBit)) {
		if (HIH8120_OK != hih8120_measure())
		{
			printShared("Temp task failed to work again!");
		}
		else {
			printShared("Temp: %f\n", hih8120_getTemperature());
			printShared("Humidity: %f\n", hih8120_getHumidity());

			self->tempMeasurementCount++;

			self->averageTemp += (int16_t)hih8120_getTemperature();

			self->averageHumidity += (int16_t)hih8120_getHumidity();


			printShared("Measurement number: %d \n", self->tempMeasurementCount);
			printShared("Average temperature: %d \n", self->averageTemp / self->tempMeasurementCount);
			printShared("Average humidity: %d \n", self->averageHumidity / self->tempMeasurementCount);

		}
	}
}
