
/*
 * TemperatureHandler.h
 *
 * Created: 4/25/2021 3:54:05 PM
 *  Author: Artis
 */ 
#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct temperatureHandler* temperatureHandler_t;

temperatureHandler_t temperatureHandler_create();
temperatureHandler_t tempHander_destroy(temperatureHandler_t self);
void getTemperatureMesurements(temperatureHandler_t self);
int16_t  getTemperature(temperatureHandler_t self);
int16_t  getHumidity(temperatureHandler_t self);