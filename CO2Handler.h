
/*
 * CO2Handler.h
 *
 * Created: 4/25/2021 5:10:56 PM
 *  Author: 
 */ 

#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct CO2Handler* CO2Handler_t;

CO2Handler_t co2_create();
void initialiseCO2Sensor();
void getCO2Mesurement(CO2Handler_t self);
uint16_t getCO2(CO2Handler_t self);
void start_co2_task(void* self);
void reset_averageCO2(CO2Handler_t self);
