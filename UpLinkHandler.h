/*
 * UpLinkHandler.h
 *
 * Created: 12/05/2021 10:23:40
 *  Author: Cata
 */ 

void lora_handler_initialise(UBaseType_t lora_handler_task_priority, temperatureHandler_t temperatureHandlerObj, lightReader_t lightReaderObj, CO2Handler_t CO2HandlerObj);