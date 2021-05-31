#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>

#include <tsl2591.h>
#include "event_groups.h"
#include "task.h"
#include "lightReader.h"
#include "SharedPrint.h"
#include "event_groups.h"

EventGroupHandle_t task_startGroup;
EventBits_t _readyBit;

void light_task_start(void* self);
void tsl2591Callback(tsl2591_returnCode_t rc, lightReader_t self);

typedef struct lightReader
{
	uint16_t lux;
	int lightMeasurementCount;
	uint16_t averageLight;

}lightReader;

lightReader_t initialiseLightDriver(UBaseType_t light_task_priority, EventGroupHandle_t eventBits, EventBits_t bits) {
	lightReader_t new_reader = calloc(1, sizeof(lightReader));

	if (new_reader == NULL) return NULL;
	new_reader->lux = 0;
	new_reader->averageLight = 0;
	new_reader->lightMeasurementCount = 0;

	_readyBit = bits;
	task_startGroup = eventBits;

	if (TSL2591_OK == tsl2591_initialise(tsl2591Callback))
	{
		printf("Return code for light sensor is: %s", tsl2591_initialise(tsl2591Callback));
	}

	if (TSL2591_OK == tsl2591_enable())
	{

	}
	light_initializeTask(light_task_priority, new_reader);

	return new_reader;
}

void reset_averageLight(lightReader_t self) {
	self->lightMeasurementCount;
	self->averageLight;
}

void light_initializeTask(UBaseType_t lightPriority, lightReader_t self)
{

	xEventGroupSetBits(task_startGroup, _readyBit);

	xTaskCreate(
		light_task_start
		, "lightTask"
		, configMINIMAL_STACK_SIZE + 100
		, (void*)self
		, lightPriority
		, NULL
	);
}

void tsl2591Callback(tsl2591_returnCode_t rc, lightReader_t self)
{
	uint16_t _tmp;
	float _lux;

	switch (rc)
	{
	case TSL2591_DATA_READY:
		if (TSL2591_OK == (rc = tsl2591_getFullSpectrumRaw(&_tmp)))
		{
		}
		else if (TSL2591_OVERFLOW == rc)
		{

			printShared("\nFull spectrum overflow - change gain and integration time\n");


		}

		if (TSL2591_OK == (rc = tsl259_getVisibleRaw(&_tmp)))
		{
		}
		else if (TSL2591_OVERFLOW == rc)
		{

			printShared("Visible overflow - change gain and integration time\n");


		}

		if (TSL2591_OK == (rc = tsl2591_getInfraredRaw(&_tmp)))
		{

		}
		else if (TSL2591_OVERFLOW == rc)
		{

			printShared("Infrared overflow - change gain and integration time\n");


		}

		if (TSL2591_OK == (rc = tsl2591_getLux(&_lux)))
		{

			printShared("Light current float: %f\n", _lux);
			if (_lux != 0) {
				self->lightMeasurementCount++;
			}
			self->lux = _lux;

		}
		else if (TSL2591_OVERFLOW == rc)
		{

			printShared("Lux overflow - change gain and integration time\n");

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

void getLightMeasurements(lightReader_t self) {

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500 / portTICK_PERIOD_MS; // 500 ms

	xTaskDelayUntil(&xLastWakeTime, xFrequency);

	if (TSL2591_OK != tsl2591_fetchData())
	{

	}
	else
	{
		tsl2591Callback(TSL2591_DATA_READY, self);
	}
}

uint16_t getLight(lightReader_t self) {
	return self->lux;
}

void light_task_start(void* self) {
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(15500UL);
	xLastWakeTime = xTaskGetTickCount();

	for (;;)
	{
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
		light_handler_task((lightReader_t)self);
	}
}

void light_handler_task(lightReader_t self)
{
	EventBits_t readyBits = xEventGroupWaitBits(task_startGroup,
		_readyBit,
		pdFALSE,
		pdTRUE,
		portMAX_DELAY);

	if ((readyBits & (_readyBit)) == (_readyBit)) {
		getLightMeasurements(self);

		self->averageLight += self->lux;

		printShared("Measurement number of light: %d \n", self->lightMeasurementCount);
		printShared("Value of average light: %d \n", self->averageLight / self->lightMeasurementCount);
		printShared("Value of current light: %d \n", self->lux);
	}
}

