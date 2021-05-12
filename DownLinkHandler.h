/*
 * DownLinkHandler.h
 *
 * Created: 12/05/2021 10:23:21
 *  Author: Cata
 */ 

#pragma once

#include <stddef.h>
#include <stdio.h>

#include <ATMEGA_FreeRTOS.h>

#include <lora_driver.h>
#include <status_leds.h>

void DownLinkHandler_Create();
void DownLinkHandler_task();
//to add more if needed