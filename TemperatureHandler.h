
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
#include "event_groups.h"
typedef struct temperatureHandler* temperatureHandler_t;

temperatureHandler_t temperatureHandler_create(UBaseType_t temp_task_priority, EventGroupHandle_t eventBits, EventBits_t bits);
temperatureHandler_t tempHander_destroy(temperatureHandler_t self);
void getTemperatureMesurements(temperatureHandler_t self);
int16_t  getTemperature(temperatureHandler_t self);
uint16_t  getHumidity(temperatureHandler_t self);
void start_temperature_task(void* self);
void reset_averageTemperature(temperatureHandler_t self);