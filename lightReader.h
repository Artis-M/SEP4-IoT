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

typedef struct lightReader* lightReader_t;

lightReader_t initialiseLightDriver(); //remember arguments here
void light_destroy(lightReader_t self);
uint16_t getLight(lightReader_t self);
void getLightMeasurements();




