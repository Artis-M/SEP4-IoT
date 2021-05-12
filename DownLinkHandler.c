/*
 * DownLinkHandler.c
 *
 * Created: 12/05/2021 10:18:54
 *  Author: Cata
 */ 

#include <ATMEGA_FreeRTOS.h>
#include <stddef.h>
#include <stdio.h>
#include "DownLinkHandler.h"
//#include "Configuration.h"
#include <stdlib.h>
#include <avr/include/avr/eeprom.h>//persistence
#include <lora_driver.h>
/*Leds*/
//#include <iled.h>
#include <message_buffer.h>

static lora_driver_payload_t _downlink_Payload;

void lora_DownLinkHandler_StartTask(void* messageBuffer){
	for(;;)
	{
		lora_DownLinkHandler_Task((MessageBufferHandle_t)messageBuffer);
	}
}

void lora_DownLinkHandler_Create(UBaseType_t priority, UBaseType_t stack, MessageBufferHandle_t messageBuffer)
{
	xTaskCreate(
	lora_DownLinkHandler_StartTask
	,  (const portCHAR *)"LRDHHand" //don't know what type the constant should be
	,  stack+200
	,  (void*)messageBuffer
	,  priority
	,  NULL );
}

void lora_DownLinkHandler_Task(MessageBufferHandle_t messageBuffer)
{
	
	xMessageBufferReceive(messageBuffer,&_downlink_Payload,sizeof(lora_driver_payload_t),portMAX_DELAY);
	printf("DOWN LINK: from port: %d with %d bytes received!", _downlink_Payload.portNo, _downlink_Payload.len); // Just for Debug
	
	//We need to know the exactly what we are getting, because we need the length
	if(_downlink_Payload.len==10)
	{
		//Here we do what we want with the information		
		if((_downlink_Payload.bytes[0] << 8) + _downlink_Payload.bytes[1] == 0)
		{
			//need servo
		}
		else
		{
			for (int i=0; i<=9; i++)
			{
				eeprom_write_byte(i,_downlink_Payload.bytes[i]);
			}
		}
	}
	
}
