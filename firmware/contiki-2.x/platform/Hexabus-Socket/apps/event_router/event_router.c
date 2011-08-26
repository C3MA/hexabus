/*
 * metering.c
 *
 * Created on: 20.08.2011
 * Author: Ollo
 */

#include "event_router.h"

#include "contiki.h"
#include "contiki-lib.h"

#define DEBUG 1

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif


PROCESS(event_router_process, "HEXABUS event_router");
AUTOSTART_PROCESSES(&event_router_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(event_router_process, ev, data) {
	// variables are declared static to ensure their values are kept
	
	// between kernel calls.
	
	// temp variables
	
	
	PROCESS_BEGIN();

	
	PRINTF("event_router started.\n");
	while (1) {
		// wait until we get an event
		PROCESS_WAIT_EVENT();
		if (ev == PROCESS_EVENT_EXITED) continue; //Why?

 	      	// display it
        	PRINTF("Event empfangen: vendor=%x, device=%x, class=%x, payload=%x\n", ((event_data_t*)data)->vendorID, ((event_data_t*)data)->deviceID, ((event_data_t*)data)->deviceClass, ((event_data_t*)data)->payload.Word);
	}
	
	PROCESS_END();
}

