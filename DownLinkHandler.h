/*
 * DownLinkHandler.h
 *
 * Created: 12/05/2021 10:23:21
 *  Author: 
 */ 

#pragma once

#include <stddef.h>
#include <stdio.h>

#include <ATMEGA_FreeRTOS.h>

#include <lora_driver.h>
#include <status_leds.h>
#include "event_groups.h"
void DownLinkHandler_Create(UBaseType_t priority, UBaseType_t stack, MessageBufferHandle_t messageBuffer);
void DownLinkHandler_StartTask(void* messageBuffer);
//to add more if needed