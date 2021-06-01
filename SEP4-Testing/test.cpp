#include "gtest/gtest.h"
#include "fff.h"
DEFINE_FFF_GLOBALS;

extern "C"
{
#include "tsl2591.h"
#include "hih8120.h"
#include "mh_z19.h"
#include <TemperatureHandler.h>
#include <lightReader.h>
#include <CO2Handler.h>
#include <SharedPrint.h>
#include "ATMEGA_FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

#define BIT_TEMP_READY (1 << 0)
#define BIT_CO2_READY (1 << 1)
#define BIT_LIGHT_READY (1 << 2)
}

typedef void (*cb)(tsl2591_returnCode_t);


FAKE_VALUE_FUNC(tsl2591_returnCode_t, tsl2591_enable);

FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_takeMeassuring);

FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_initialise);

FAKE_VALUE_FUNC(BaseType_t, xTaskCreate, TaskFunction_t, const char*, configSTACK_DEPTH_TYPE, void*, UBaseType_t, TaskHandle_t*);
FAKE_VOID_FUNC(xSemaphoreGive, SemaphoreHandle_t);
FAKE_VOID_FUNC(xTaskDelayUntil, TickType_t*, TickType_t);
FAKE_VALUE_FUNC(TickType_t, xTaskGetTickCount);
FAKE_VALUE_FUNC(EventGroupHandle_t, xEventGroupCreate);

class Test_Sensors :public::testing::Test
{
protected:
	void SetUp() override
	{
		RESET_FAKE(tsl2591_enable);
		RESET_FAKE(mh_z19_takeMeassuring);
		RESET_FAKE(hih8120_initialise);
		RESET_FAKE(xTaskCreate);
		RESET_FAKE(xTaskDelayUntil);
		RESET_FAKE(xTaskGetTickCount)
		RESET_FAKE(xSemaphoreGive);
		FFF_RESET_HISTORY();
	}
	void TearDown() override
	{

	}
};

TEST_F(Test_Sensors, Test_if_light_sensor_is_enabled)
{

	EventGroupHandle_t taskReadyBits = xEventGroupCreate();

	initialiseLightDriver(3, taskReadyBits, BIT_LIGHT_READY);

	EXPECT_EQ(1, tsl2591_enable_fake.call_count);

}

TEST_F(Test_Sensors, Test_if_co2_sensor_is_enabled)
{

	EventGroupHandle_t taskReadyBits = xEventGroupCreate();

	CO2Handler_t co2Handler = co2_create(3, taskReadyBits, BIT_CO2_READY);

	EXPECT_EQ(1, mh_z19_takeMeassuring_fake.call_count);

}

TEST_F(Test_Sensors, Test_if_temp_sensor_is_enabled)
{

	EventGroupHandle_t taskReadyBits = xEventGroupCreate();

	temperatureHandler_t tempHandler = temperatureHandler_create(3, taskReadyBits, BIT_CO2_READY);

	EXPECT_EQ(1, hih8120_initialise_fake.call_count);

}


TEST_F(Test_Sensors, Test_if_sharedPrint_returns){

	printShared("test");
	EXPECT_EQ(1, xSemaphoreGive_fake.call_count);

}