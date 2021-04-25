/*
 * lightReader.h
 *
 * Created: 25/04/2021 16:17:57
 *  Author: Cata
 */ 

#pragma once

#include <ATMEGA_FreeRTOS.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "task.h"
#include "event_groups.h"

//typedef struct  lightReader* lightReader_t;

lightReader_t initialiseLightDriver();
void destroy();
void getLight();
void getMeasurements();



