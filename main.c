#include <stdio.h>
#include <avr/io.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio_driver.h>
#include <serial.h>

#include <hih8120.h>
#include <tsl2591.h>

#include <lora_driver.h>
#include <status_leds.h>

#include "event_groups.h"
#include "SharedPrint.h"

#include "CO2Handler.h"
#include "TemperatureHandler.h"
#include "LightReader.h"

MessageBufferHandle_t downLinkMessageBufferHandle;
EventGroupHandle_t taskReadyBits = NULL;

#define BIT_TEMP_READY (1 << 0)
#define BIT_CO2_READY (1 << 1)
#define BIT_LIGHT_READY (1 << 2)

temperatureHandler_t temperatureHandler;
lightReader_t lightReader;
CO2Handler_t CO2Handler;


void create_tasks_and_semaphores(void)
{
	create_shared_print();
	createSensors(taskReadyBits, BIT_TEMP_READY, BIT_CO2_READY, BIT_LIGHT_READY);
	lora_handler_initialise(2, temperatureHandler, lightReader, CO2Handler);
	lora_DownLinkHandler_Create(3, configMINIMAL_STACK_SIZE, downLinkMessageBufferHandle);
}


void initialiseSystem()
{
	taskReadyBits = xEventGroupCreate();

	downLinkMessageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t) * 2);

	stdio_initialise(ser_USART0);

	status_leds_initialise(5);

	rc_servo_initialise();

	lora_driver_initialise(ser_USART1, downLinkMessageBufferHandle);

	create_tasks_and_semaphores();

}

void createSensors(EventGroupHandle_t taskReadyBits, EventBits_t temp_bit, EventBits_t co2_bit, EventBits_t light_bit) {
	temperatureHandler = temperatureHandler_create(3, taskReadyBits, temp_bit);
	lightReader = initialiseLightDriver(3, taskReadyBits, light_bit);
	CO2Handler = co2_create(3, taskReadyBits, co2_bit);
}

/*-----------------------------------------------------------*/
int main(void)
{
	initialiseSystem();
	printf("Program Starting!\n");
	vTaskStartScheduler();
	while (1){
		
	}
}