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


// define two Tasks
void task1(void *pvParameters);
void task2(void *pvParameters);

// define semaphore handle
SemaphoreHandle_t xTestSemaphore;
bool initializedTemp = false;

// Prototype for LoRaWAN handler
void lora_handler_initialise(UBaseType_t lora_handler_task_priority);

/*-----------------------------------------------------------*/
void create_tasks_and_semaphores(void)
{
	// Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
	// because it is sharing a resource, such as the Serial port.
	// Semaphores should only be used whilst the scheduler is running, but we can set it up here.
	if ( xTestSemaphore == NULL )  // Check to confirm that the Semaphore has not already been created.
	{
		xTestSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore.
		if ( ( xTestSemaphore ) != NULL )
		{
			xSemaphoreGive( ( xTestSemaphore ) );  // Make the mutex available for use, by initially "Giving" the Semaphore.
		}
	}

	xTaskCreate(
	task1
	,  "Task1"  // A name just for humans
	,  configMINIMAL_STACK_SIZE  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  NULL
	,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );

	xTaskCreate(
	task2
	,  "Task2"  // A name just for humans
	,  configMINIMAL_STACK_SIZE  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  NULL
	,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );
}

/*-----------------------------------------------------------*/
void task1( void *pvParameters )
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500/portTICK_PERIOD_MS; // 500 ms

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{
		
		xTaskDelayUntil( &xLastWakeTime, xFrequency );

		int humidity = 0;
		int temperature = 0;
		int light = 0;
		float light1 = 0.0;
		//char *s;
		
		if ( HIH8120_OK != hih8120_wakeup() )
		{
			// Something went wrong
			// Investigate the return code further
			puts("Task1 failed to work");
		}
		
		xTaskDelayUntil( &xLastWakeTime, xFrequency );

		
		if ( HIH8120_OK !=  hih8120_measure() )
		{
			// Something went wrong
			// Investigate the return code further
			printf("%d", initializedTemp);
			puts("Task1 failed to work again");
		}
		
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		
		if ( TSL2591_OK == tsl2591_enable() )
		{
			printf("Light enabled");
			// The power up command is now send to the sensor - it can be powered down with a call to tsl2591_disable()
		}
		
		if ( TSL2591_OK != tsl2591_fetchData() )
		{
			printf("Light not fetched");
			// Something went wrong
			// Investigate the return code further
		}
		else
		{
			
			light = (int)(1000 * tsl2591_getLux(&light1));
			printf("%d bbbbbbbbbbbbbbbbbbbbb", light);
		}

		humidity = hih8120_getHumidity();
		temperature = hih8120_getTemperature();

		puts("Task1"); // stdio functions are not reentrant - Should normally be protected by MUTEX
		printf("%d aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", humidity);
		//dtostrf(humidity, 4, 2, s);
		//sprintf("%s aaaaaaaaaaaa", s);
		PORTA ^= _BV(PA0);
		
	}
}

/*-----------------------------------------------------------*/
void task2( void *pvParameters )
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1000/portTICK_PERIOD_MS; // 1000 ms

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		puts("Task2"); // stdio functions are not reentrant - Should normally be protected by MUTEX
		PORTA ^= _BV(PA7);
	}
}

/*-----------------------------------------------------------*/
void initialiseSystem()
{
	// Set output ports for leds used in the example
	DDRA |= _BV(DDA0) | _BV(DDA7);

	// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);
	// Let's create some tasks
	create_tasks_and_semaphores();

	

	// vvvvvvvvvvvvvvvvv BELOW IS LoRaWAN initialisation vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// Status Leds driver
	status_leds_initialise(5); // Priority 5 for internal task
	// Initialise the LoRaWAN driver without down-link buffer
	lora_driver_initialise(1, NULL);
	// Create LoRaWAN task and start it up with priority 3
	lora_handler_initialise(3);
}

void initialiseSensors(){
	if ( HIH8120_OK == hih8120_initialise() )
	{
		printf("Temp sensor initialized");
		printf("_______________________");
		initializedTemp = true;
	}
}

void tsl2591Callback(tsl2591_returnCode_t rc)
{
	uint16_t _tmp;
	float _lux;
	switch (rc)
	{
		case TSL2591_DATA_READY:
		if ( TSL2591_OK == (rc = tsl2591_getFullSpectrumRaw(&_tmp)) )
		{
			printf("\nFull Raw:%04X\n", _tmp);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("\nFull spectrum overflow - change gain and integration time\n");
		}
		
		if ( TSL2591_OK == (rc = tsl259_getVisibleRaw(&_tmp)) )
		{
			printf("Visible Raw:%04X\n", _tmp);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("Visible overflow - change gain and integration time\n");
		}
		
		if ( TSL2591_OK == (rc = tsl2591_getInfraredRaw(&_tmp)) )
		{
			printf("Infrared Raw:%04X\n", _tmp);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("Infrared overflow - change gain and integration time\n");
		}
		
		if ( TSL2591_OK == (rc = tsl2591_getLux(&_lux)) )
		{
			printf("Lux: %5.4f\n", _lux);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("Lux overflow - change gain and integration time\n");
		}
		break;
		
		case TSL2591_OK:
		// Last command performed successful
		break;
		
		case TSL2591_DEV_ID_READY:
		// Dev ID now fetched
		break;
		
		default:
		break;
	}
}

void initliazeLight()
{
	if ( TSL2591_OK == tsl2591_initialise(tsl2591Callback) )
	{
		// Driver initilised OK
		// Always check what tsl2591_initialise() returns
		printf("Light initialized\n");
	}
}

/*-----------------------------------------------------------*/
int main(void)
{
	initialiseSystem(); // Must be done as the very first thing!!
	printf("Program Started!!\n");
	initialiseSensors();
	initliazeLight();
	vTaskStartScheduler(); // Initialise and run the freeRTOS scheduler. Execution should never return from here.

	/* Replace with your application code */
	while (1)
	{
	}
}

