
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

typedef struct temperatureHandler* TemperatureHandler_t;

temperatureHandler _create(UBaseType_t priority, UBaseType_t stack, EventGroupHandle_t startMeasureEventGroup, EventBits_t startMeasureBit,
EventGroupHandle_t readyEventGroup, EventBits_t readyBit);
void initialiseTempDriver();
void getMeasurements();
float getTemperature();
float getHumidity();