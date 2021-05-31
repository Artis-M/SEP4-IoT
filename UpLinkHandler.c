#include <stddef.h>
#include <stdio.h>

#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>

#include <lora_driver.h>
#include <status_leds.h>

#include "CO2Handler.h"
#include "TemperatureHandler.h"
#include "LightReader.h"
#include "SharedPrint.h"

#define LORA_appEUI "689DF9DF68156742"
#define LORA_appKEY "B09F779D3DF66B89B996955E3B4ED977"

static char _out_buf[100];

void lora_handler_task(void* pvParameters);

static lora_driver_payload_t _uplink_payload;

temperatureHandler_t temperatureHandler;
lightReader_t lightReader;
CO2Handler_t CO2Handler;

void lora_handler_initialise(UBaseType_t lora_handler_task_priority, temperatureHandler_t temperatureHandlerObj, lightReader_t lightReaderObj, CO2Handler_t CO2HandlerObj)
{

	temperatureHandler = temperatureHandlerObj;
	lightReader = lightReaderObj;
	CO2Handler = CO2HandlerObj;


	printf("SETTING UP LORAWAN");
	xTaskCreate(
		lora_handler_task
		, "LRHand"
		, configMINIMAL_STACK_SIZE + 200
		, NULL
		, lora_handler_task_priority
		, NULL);
}

static void _lora_setup(void)
{
	lora_driver_returnCode_t rc;
	status_leds_slowBlink(led_ST2); // OPTIONAL: Led the green led blink slowly while we are setting up LoRa

	// Factory reset the transceiver
	printf("FactoryReset >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_rn2483FactoryReset()));

	// Configure to EU868 LoRaWAN standards
	printf("Configure to EU868 >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_configureToEu868()));

	// Get the transceivers HW EUI
	rc = lora_driver_getRn2483Hweui(_out_buf);
	printf("Get HWEUI >%s<: %s\n", lora_driver_mapReturnCodeToText(rc), _out_buf);

	// Set the HWEUI as DevEUI in the LoRaWAN software stack in the transceiver
	printf("Set DevEUI: %s >%s<\n", _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setDeviceIdentifier(_out_buf)));

	// Set Over The Air Activation parameters to be ready to join the LoRaWAN
	printf("Set OTAA Identity appEUI:%s appKEY:%s devEUI:%s >%s<\n", LORA_appEUI, LORA_appKEY, _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setOtaaIdentity(LORA_appEUI, LORA_appKEY, _out_buf)));

	// Save all the MAC settings in the transceiver
	printf("Save mac >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_saveMac()));

	// Enable Adaptive Data Rate
	printf("Set Adaptive Data Rate: ON >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_setAdaptiveDataRate(LORA_ON)));

	// Set receiver window1 delay to 500 ms - this is needed if down-link messages will be used
	printf("Set Receiver Delay: %d ms >%s<\n", 500, lora_driver_mapReturnCodeToText(lora_driver_setReceiveDelay(500)));

	// Join the LoRaWAN
	uint8_t maxJoinTriesLeft = 10;

	do {
		rc = lora_driver_join(LORA_OTAA);
		printf("Join Network TriesLeft:%d >%s<\n", maxJoinTriesLeft, lora_driver_mapReturnCodeToText(rc));

		if (rc != LORA_ACCEPTED)
		{
			// Make the red led pulse to tell something went wrong
			status_leds_longPuls(led_ST1); // OPTIONAL
			// Wait 5 sec and lets try again
			vTaskDelay(pdMS_TO_TICKS(5000UL));
		}
		else
		{
			break;
		}
	} while (--maxJoinTriesLeft);

	if (rc == LORA_ACCEPTED)
	{
		// Connected to LoRaWAN :-)
		// Make the green led steady
		status_leds_ledOn(led_ST2); // OPTIONAL
	}
	else
	{
		// Something went wrong
		// Turn off the green led
		status_leds_ledOff(led_ST2); // OPTIONAL
		// Make the red led blink fast to tell something went wrong
		status_leds_fastBlink(led_ST1); // OPTIONAL

		// Lets stay here
		while (1)
		{
			taskYIELD();
		}
	}
}

void lora_handler_task(void* pvParameters)
{

	lora_driver_resetRn2483(1);
	vTaskDelay(2);
	lora_driver_resetRn2483(0);

	vTaskDelay(150);

	lora_driver_flushBuffers();

	_lora_setup();

	_uplink_payload.len = 8;
	_uplink_payload.portNo = 2;

	TickType_t xLastWakeTime;

	const TickType_t xFrequency = pdMS_TO_TICKS(300000UL);
	xLastWakeTime = xTaskGetTickCount();

	for (;;)
	{
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
		printShared("Sending data to LORA \n");

		getTemperatureMesurements(temperatureHandler);
		uint16_t hum = getHumidity(temperatureHandler) + 4096;
		int16_t temp = getTemperature(temperatureHandler) + 12288;
		reset_averageTemperature(temperatureHandler);
		uint16_t lux = getLight(lightReader) + 16384;
		reset_averageLight(lightReader);
		uint16_t co2_ppm = getCO2(CO2Handler) + 8192;
		reset_averageCO2(CO2Handler);

		_uplink_payload.bytes[0] = hum >> 8;
		_uplink_payload.bytes[1] = hum & 0xFF;

		_uplink_payload.bytes[2] = temp >> 8;
		_uplink_payload.bytes[3] = temp & 0xFF;

		_uplink_payload.bytes[4] = co2_ppm >> 8;
		_uplink_payload.bytes[5] = co2_ppm & 0xFF;

		_uplink_payload.bytes[6] = lux >> 8;
		_uplink_payload.bytes[7] = lux & 0xFF;



		//For debugging purposes
		for (int i = 0; i < 7; i++)
		{
			if (i > 0) printf(":");
			printShared("%02X", _uplink_payload.bytes[i]);
		}

		status_leds_shortPuls(led_ST4);

		printShared("Upload Message >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_sendUploadMessage(false, &_uplink_payload)));

	}
}
