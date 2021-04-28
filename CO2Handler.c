
/*
 * CO2Handler.c
 *
 * Created: 4/25/2021 5:17:54 PM
 *  Author: Artis
 */ 
#include <mh_z19.h>
#include <stdio.h>
uint16_t ppm;
mh_z19_returnCode_t rc;

void initialiseCO2Sensor(){
	mh_z19_initialise(ser_USART3); 
}

uint16_t getCO2Mesurement(){
	rc = mh_z19_takeMeassuring();
	if (rc != MHZ19_OK)
	{
		printf("CO2 Measurement failed.");
		return 0;
	}
	return rc;
}