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

//static EventGroupHandle_t _startMeasureEventGroup;
//static EventBits_t _startMeasureBit;

//static EventGroupHandle_t _readyMeasuringEventGroup;
//static EventBits_t _readyBit;

//typedef struct lightReader_t lightReader;

void initialiseLightDriver(){
	if ( TSL2591_OK == tsl2591_initialise(tsl2591Callback) )
	{
		printf("Light driver initialized");
		printf("_______________________");
	}
}

void tsl2591Callback(tsl2591_returnCode_t rc)
{
	uint16_t _tmp;
	float _lux;
	switch (rc)
	{
		case TSL2591_DATA_READY:
		if ( TSL2591_OK == (rc = tsl2591_getFullSpectrumRaw(&_tmp)) )
		{
			printf("\nFull Raw:%04X\n", _tmp);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("\nFull spectrum overflow - change gain and integration time\n");
		}
		
		if ( TSL2591_OK == (rc = tsl259_getVisibleRaw(&_tmp)) )
		{
			printf("Visible Raw:%04X\n", _tmp);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("Visible overflow - change gain and integration time\n");
		}
		
		if ( TSL2591_OK == (rc = tsl2591_getInfraredRaw(&_tmp)) )
		{
			printf("Infrared Raw:%04X\n", _tmp);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("Infrared overflow - change gain and integration time\n");
		}
		
		if ( TSL2591_OK == (rc = tsl2591_getLux(&_lux)) )
		{
			printf("Lux: %5.4f\n", _lux);
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

void getMeasurements(){
	
	if ( TSL2591_OK == tsl2591_enable() )
	{
		puts("Light enabled");
		// The power up command is now send to the sensor - it can be powered down with a call to tsl2591_disable()
	}
	
	if ( TSL2591_OK != tsl2591_fetchData() )
	{
		puts("Light not fetched");
		// Something went wrong
		// Investigate the return code further
	}
	else
	{
		tsl2591_getLux(&light); //sth wrong here. Repair later
		printf("The Light Data Received from the sensor is : %2.2f \n", light);
	}
	
	
}

float getLight(){
	return tsl2591_getLux();
}

