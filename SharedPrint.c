#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <stdio.h>
#include <stdarg.h>

SemaphoreHandle_t sharedMutex;

void create_shared_print(){
	sharedMutex = xSemaphoreCreateMutex();
 }

void printShared(const char *format, ...){
	  va_list lst;
	if(xSemaphoreTake( sharedMutex,pdMS_TO_TICKS(400))==pdTRUE){
		va_start(lst, format);
		vprintf(format, lst);
		xSemaphoreGive( sharedMutex);
	}
	else{
		
	}	
}