/*
 *  event_router.h
 *
 *  Created on: 24.08.2011
 *  Author: tobi
 */

#ifndef EVENT_ROUTER_H_
#define EVENT_ROUTER_H_


#include "process.h"
#include <stdint.h>
#include <string.h>

#define ER_PAYLOAD_TYPE_WORD 0x0;
#define ER_PAYLOAD_TYPE_DWORD 0x1;
#define ER_PAYLOAD_TYPE_CHAR 0x2;
#define ER_PAYLOAD_TYPE_STRING 0x3;

typedef union {
	uint8_t 	Word;
	uint16_t 	DWord;
	
	char		Character;
	char*		String;
} er_payload_t;

typedef struct {
	uint8_t 	vendorID;
	uint8_t 	deviceID;
	uint8_t		deviceClass;
	uint8_t		type;
	er_payload_t	payload;
} event_data_t;


/* Variables: the application specific event value */


PROCESS_NAME(event_router_process);


#endif /* EVENT_ROUTER_H_ */
