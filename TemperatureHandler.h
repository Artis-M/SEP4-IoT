
/*
 * TemperatureHandler.h
 *
 * Created: 4/25/2021 3:54:05 PM
 *  Author: Artis
 */ 
#pragma once

typedef struct TemperatureHandler* TemperatureHandler_t;

void initialiseTempDriver();
void getMeasurements();
float getTemperature();
float getHumidity();