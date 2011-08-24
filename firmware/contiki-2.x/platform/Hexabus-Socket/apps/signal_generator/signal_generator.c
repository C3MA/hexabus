/*
 * metering.c
 *
 * Created on: 20.08.2011
 * Author: Ollo
 */

#include "signal_generator.h"

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

static uint16_t value = 0;
#define LIMIT_HIGH 2000
#define LIMIT_LOW 20

uint16_t sg_getValue() {
	return value;
}

PROCESS(signal_generator_process, "HEXABUS signal generator");
AUTOSTART_PROCESSES(&signal_generator_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(signal_generator_process, ev, data) {
	// variables are declared static to ensure their values are kept
	
	// between kernel calls.
	static struct etimer timer;
	static uint8_t direction = 1;
	
	// temp variables

	
	PROCESS_BEGIN();

	
	// set the etimer module to generate an event every ten seconds.
	etimer_set(&timer, CLOCK_CONF_SECOND/16);

	PRINTF("signal generator started.\n");
	while (1) {
		PROCESS_WAIT_EVENT();
		
		if(ev == PROCESS_EVENT_TIMER)
		{

			if (direction) 
			{
				value++;
				if ( value >= LIMIT_HIGH )
					direction = 0;
			}
			else
			{
				value--;
				if ( value <= LIMIT_LOW )
					direction = 1;
			}
			etimer_reset(&timer);
		}
	}
	
	PROCESS_END();
}

