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
#include "Servo.h"
//#include "Configuration.h"
#include <stdlib.h>
#include <avr/eeprom.h>//persistence
#include <lora_driver.h>
/*Leds*/
//#include <iled.h>
#include <message_buffer.h>

static lora_driver_payload_t _downlink_Payload;

void lora_DownLinkHandler_StartTask(void* messageBuffer);

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
	,  "LRDHDownHand"
	,  stack+200
	,  (void*)messageBuffer
	,  priority
	,  NULL );
}

void lora_DownLinkHandler_Task(MessageBufferHandle_t messageBuffer)
{
			printf("Downlink Task Started \n");
			xMessageBufferReceive(messageBuffer,&_downlink_Payload,sizeof(lora_driver_payload_t),portMAX_DELAY);
			printf("DOWN LINK: from port: %d with %d bytes received! \n", _downlink_Payload.portNo, _downlink_Payload.len); // Just for Debug
			
			//We need to know the exactly what we are getting, because we need the length
			if(_downlink_Payload.len==8)
			{
				int i;
				for (i = 0; i < 7; i++)
				{
					if (i > 0) printf(":");
					printf("%02X", _downlink_Payload.bytes[i]);
				}
				
				//Here we do what we want with the information
				if((_downlink_Payload.bytes[0] << 8) + _downlink_Payload.bytes[1] == 0)
				{
					if(_downlink_Payload.bytes[2] + _downlink_Payload.bytes[3] == 1)
					servo_setPosition(0, 100); //fully open to the right
					else if(_downlink_Payload.bytes[2] + _downlink_Payload.bytes[3] == 0)
					{
						servo_setPosition(0, 0); //closed
					}
				}
				else
				{
					for (int i=0; i<=7; i++)
					{
						eeprom_write_byte(i,_downlink_Payload.bytes[i]);
					}
				}
			}
}
