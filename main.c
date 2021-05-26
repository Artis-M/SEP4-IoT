/*
* main.c
* Author : IHA
*
* Example main file including LoRaWAN setup
* Just for inspiration :)
*/

#include <stdio.h>
#include <avr/io.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio_driver.h>
#include <serial.h>

//Hardware drivers
#include <hih8120.h>
#include <tsl2591.h>

// Needed for LoRaWAN
#include <lora_driver.h>
#include <status_leds.h>

#include "SharedPrint.h"
// define semaphore handle

bool initializedTemp = false;
MessageBufferHandle_t downLinkMessageBufferHandle;

// Prototype for LoRaWAN handler
void lora_handler_initialise(UBaseType_t lora_handler_task_priority);

/*-----------------------------------------------------------*/
void create_tasks_and_semaphores(void)
{
lora_handler_initialise(2);
createSensors();
lora_DownLinkHandler_Create(3, configMINIMAL_STACK_SIZE, downLinkMessageBufferHandle);
}


void initialiseSystem()
{
	create_shared_print();
	downLinkMessageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t)*2);
	// Set output ports for leds used in the example
	//DDRA |= _BV(DDA0) | _BV(DDA7);
FREERTOS_CONFIG_H
	// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);

	
	status_leds_initialise(5); 
	
	rc_servo_initialise();
	
	lora_driver_initialise(ser_USART1, downLinkMessageBufferHandle);
	
	create_tasks_and_semaphores();

}


/*-----------------------------------------------------------*/
int main(void)
{
	initialiseSystem(); // Must be done as the very first thing
	printf("Program Started!!\n");
	vTaskStartScheduler(); // Initialise and run the freeRTOS scheduler. Execution should never return from here.
	/* Replace with your application code */
	while (1)
	{
	}
}