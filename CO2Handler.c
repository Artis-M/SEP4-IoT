/*
/*
 * CO2Handler.c
 *
 * Created: 4/25/2021 5:17:54 PM
 *  Author: 
 */ 
#include <mh_z19.h>
#include <stdio.h>
#include "CO2Handler.h"

uint16_t ppm;
mh_z19_returnCode_t rc;
void myCo2CallBack(uint16_t ppmCall);

typedef struct CO2Handler
{
	uint16_t co2ppm;
} CO2Handler;

CO2Handler_t co2_create(){
	CO2Handler_t _new_reader = calloc(1, sizeof(CO2Handler));
	if (_new_reader == NULL){
		return NULL;
	}
	_new_reader->co2ppm = 0;
	initialiseCO2Sensor();
	return _new_reader;
}

void initialiseCO2Sensor(){
	mh_z19_initialise(ser_USART3); 
	mh_z19_injectCallBack(myCo2CallBack);
}

void co2_executeTask(CO2Handler_t self)
{
	for(;;)
	{
		getCO2Mesurement(self);
	}
}

void getCO2Mesurement(CO2Handler_t self){
	rc = mh_z19_takeMeassuring();
	if (rc != MHZ19_OK)
	{
		printf("CO2 Measurement failed.");
	}
	self->co2ppm = ppm;
}

void myCo2CallBack(uint16_t ppmCall)
{
	printf("____________________________________________________________ \n");
	printf("CO2: %d \n", ppm);
	ppm = ppmCall;
}

uint16_t getCO2(CO2Handler_t self){
	getCO2Mesurement(self);
	return self->co2ppm;
}