/*
/*
 * CO2Handler.c
 *
 * Created: 4/25/2021 5:17:54 PM
 *  Author: Artis
 */ 
#include <mh_z19.h>
#include <stdio.h>

/*

typedef struct CO2Handler
{
	uint16_t ppm;
}CO2Handler;

mh_z19_returnCode_t rc;

void initialiseCO2Sensor(){
	mh_z19_initialise(ser_USART3); 
	mh_z19_injectCallBack(myCo2CallBack);
}

void getCO2Mesurement(CO2Handler self){
	rc = mh_z19_takeMeassuring();
	if (rc != MHZ19_OK)
	{
		printf("CO2 Measurement failed.");
	}
}

void myCo2CallBack(uint16_t ppm)
{
	CO2Handler self;
	self->ppm = ppm;
}

uint16_t getCO2(CO2Handler self){
	return self->ppm;
}*/