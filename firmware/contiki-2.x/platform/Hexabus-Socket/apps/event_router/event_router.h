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

/* Variables: the application specific event value */
extern process_event_t event_data_ready;

PROCESS_NAME(event_router_process);


#endif /* EVENT_ROUTER_H_ */
