#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <stdarg.h>

SemaphoreHandle_t sharedMutex;


void create_shared_print(){
	sharedMutex = xSemaphoreCreateMutex();
}

void printShared(const char *format, ...){
	  va_list lst;
	if(xSemaphoreTake( sharedMutex,pdMS_TO_TICKS(200))==pdTRUE){
		va_start(lst, format);
		vprintf(format, lst);
		xSemaphoreGive( sharedMutex);
	}
	else{
		//printf("Something is very wrong uwu \n");
	}
	
}