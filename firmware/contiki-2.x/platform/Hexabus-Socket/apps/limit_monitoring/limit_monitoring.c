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

#include "signal_generator.h"
#include "event_router.h"

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

inline uint8_t lm_set_isActive(const lm_limit_set_t *set)
{
	return (set->mode & LM_LIMIT_SET_MODE_ACTIVE);
}

inline uint8_t lm_definition_isActive(const lm_limit_definition_t *definition)
{
	return (definition->mode & LM_LIMIT_DEFINITION_MODE_ACTIVE);
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

static void handleLimitDef(lm_limit_definition_t *def, uint16_t value) {
	if(def->mode & LM_LIMIT_DEFINITION_MODE_DIRECTION_STATE) { // limit reached
		if (def->mode & LM_LIMIT_DEFINITION_MODE_DIRECTION_HIGHLOW) { // High limit
			if ( value >= def-> limit_value) {
				
			} else {
				if (++(def->tick_counter) >= def->tick_deadband) {
					def->tick_counter = 0;
					def->mode &= ~LM_LIMIT_DEFINITION_MODE_DIRECTION_STATE;
					PRINTF("was high\n");
				}
			}
		} else { // Low limit
			if ( value <= def->limit_value) {
				
			} else {
				if (++(def->tick_counter) >= def->tick_deadband) {
					def->tick_counter = 0;
					def->mode &= ~LM_LIMIT_DEFINITION_MODE_DIRECTION_STATE;
					PRINTF("was low\n");
				}
			}
		}
	} else {
		if (def->mode & LM_LIMIT_DEFINITION_MODE_DIRECTION_HIGHLOW) { // High limit
			if ( value >= def->limit_value) {
				if (++(def->tick_counter) >= def->tick_deadband) {
					def->tick_counter = 0;
					def->mode |= LM_LIMIT_DEFINITION_MODE_DIRECTION_STATE;
					PRINTF("is high\n");
					process_post(&event_router_process, event_data_ready, &value);
				}
			}
		} else { // Low limit
			if ( value <= def->limit_value) {
				if (++(def->tick_counter) >= def->tick_deadband) {
					def->tick_counter = 0;
					def->mode |= LM_LIMIT_DEFINITION_MODE_DIRECTION_STATE;
					PRINTF("is low\n");
				}
			}
		}
	}
}

static void handleLimitSet(lm_limit_set_t *set) {
	int i;
	
	if (set->getValue != NULL) {
		uint16_t value = set->getValue();
		PRINTF("Limit set %d: value is %2d\n", set->id, value);
		for(i=0; i < LM_LIMIT_MAX_DEVICES; i++) {
			if (lm_definition_isActive(&(set->devices[i]))) {
				handleLimitDef(&(set->devices[i]), value);			
			}
		}		
	} else {
		PRINTF("Limit set %d has no getValue function.\n", set->id);
	}	
}

static void handleLimit(lm_limit_set_t *limit)
{
	if (lm_set_isActive(limit))
	{
		if (limit->cycleCounter >= limit->cycles) {
			limit->cycleCounter=0;
			handleLimitSet(limit);
		} else {
			limit->cycleCounter ++;
		}
	}	
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(limit_monitoring_process, ev, data) {
	// variables are declared static to ensure their values are kept
	
	// between kernel calls.
	static struct etimer timer;
	static lm_limit_set_t limits[LM_LIMIT_MAX_SETS];
	
	// temp variables
	int i;
	
	PROCESS_BEGIN();

	memset(limits,0,sizeof(limits));
	
	limits[0].id = 1;
	limits[0].devices[0].limit_value = 15;
	limits[0].devices[0].limit_value = 15;
	lm_set_setActive(&limits[0], 1);
	limits[0].cycles = 15;
	limits[0].getValue = metering_get_power;
	
	limits[1].id = 2;
	lm_set_setActive(&limits[1], 1);	
	limits[1].cycles = 5;
	limits[1].getValue = sg_getValue;
	limits[1].devices[0].limit_value = 1000;
	limits[1].devices[0].mode = 3; // active High Limit
	limits[1].devices[0].tick_deadband = 2;
	limits[1].devices[1].limit_value = 700;
	limits[1].devices[1].mode = 1; // active High Limit
	limits[1].devices[1].tick_deadband = 2;


	// set the etimer module to generate an event every ten seconds.
	etimer_set(&timer, CLOCK_CONF_SECOND);

	while (1) {
		PROCESS_WAIT_EVENT();
		
		if(ev == PROCESS_EVENT_TIMER)
		{
			for (i=0; i < LM_LIMIT_MAX_SETS; i++) {
				handleLimit(&limits[i]);				
			}
			etimer_reset(&timer);
		}
	}
	
	PROCESS_END();
}

