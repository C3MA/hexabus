/*
 * metering.c
 *
 * Created on: 20.08.2011
 * Author: Ollo
 */

#include "limit_monitoring.h"

#include "contiki.h"
#include "contiki-lib.h"
#include "metering.h"

/*#include <avr/io.h>
#include <util/delay.h>
#include "sys/clock.h"
#include "contiki.h"
#include "dev/leds.h"
#include <avr/eeprom.h>
#include "eeprom_variables.h"
#include "dev/leds.h"
#include "relay.h"
*/

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


PROCESS(limit_monitoring_process, "HEXABUS Limit Monitoring Process");
AUTOSTART_PROCESSES(&limit_monitoring_process);

inline void lm_setActive(lm_limit_definition_t *definition, uint8_t active)
{
	if (active)
	{
		definition->mode = definition->mode | LM_LIMIT_DEFINITION_MODE_ACTIVE;
	}
	else 
	{
		definition->mode = definition->mode & ~LM_LIMIT_DEFINITION_MODE_ACTIVE;
	}
		
}

inline void lm_setDirection(lm_limit_definition_t *definition, uint8_t highlow)
{
	if (highlow)
	{
		definition->mode = definition->mode | LM_LIMIT_DEFINITION_MODE_DIRECTION_HIGHLOW;
	}
	else 
	{
		definition->mode = definition->mode & ~LM_LIMIT_DEFINITION_MODE_DIRECTION_HIGHLOW;
	}
}

inline uint8_t lm_set_isActive(lm_limit_set_t *set)
{
	return (set->mode & LM_LIMIT_SET_MODE_ACTIVE);
}

inline uint8_t lm_definition_isActive(lm_limit_set_t *definition)
{
	return (definition->mode & LM_LIMIT_SET_MODE_ACTIVE);
}

inline void lm_set_setActive(lm_limit_set_t *set, uint8_t active)
{
	if (active)
	{
		set->mode = set->mode | LM_LIMIT_SET_MODE_ACTIVE;
	}
	else 
	{
		set->mode = set->mode & ~LM_LIMIT_SET_MODE_ACTIVE;
	}
	
}

static void handleLimitSet(lm_limit_set_t *set) {
	PRINTF("Limit set %d\n", set->id);
}

static void handleLimit(lm_limit_set_t *limit)
{
	if (lm_set_isActive(limit))
	{	
		PRINTF("id=%d: counter %2d \t cycles %2d\n", limit->id, 			limit->cycleCounter, limit->cycles);
		if (limit->cycleCounter >= limit->cycles) {
			limit->cycleCounter=0;
			handleLimitSet(limit);
		} else {
			limit->cycleCounter = limit->cycleCounter + 1;
		}
	}	
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(limit_monitoring_process, ev, data) {
	// variables are declared static to ensure their values are kept
	
	// between kernel calls.
	static struct etimer timer;
	static int count = 0;
	static lm_limit_set_t limits[LM_LIMIT_MAX_SETS];
	
	// temp variables
	int i;
	
	/* Initialize */
	memset(limits, 0, sizeof(limits));
	
	/* Set some static default values for testing */
	limits[0].id = 1;
	limits[0].devices[0].limit_value = 30;
	limits[0].devices[0].limit_value = 30;
	lm_set_setActive(&limits[0], 1);
	limits[0].cycles = 30;
	
	limits[1].id = 2;
	lm_set_setActive(&limits[1], 1);	
	limits[1].cycles = 12;
	
	PROCESS_BEGIN();
	
	
	// set the etimer module to generate an event every ten seconds.
	etimer_set(&timer, CLOCK_CONF_SECOND);
	PRINTF("Limit Monitoring\n");
	while (1) {
		PROCESS_WAIT_EVENT();
		
		if(ev == PROCESS_EVENT_TIMER)
		{
			for (i=0; i < LM_LIMIT_MAX_SETS; i++) {
				handleLimit(&limits[i]);
			}
			PRINTF("%d.\t%2d Watt\n",  count++, 				metering_get_power());
			
			etimer_reset(&timer);
		}
	}
	
	
	PROCESS_END();
}
