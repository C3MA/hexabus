/*
 * metering.h
 *
 *  Created on: 20.08.2011
 *  Author: Ollo
 */

#ifndef LIMIT_MONITORING_H_
#define LIMIT_MONITORING_H_


#include "process.h"
#include <stdint.h>


#define LM_LIMIT_DEFINITION_MODE_ACTIVE				0x1
#define LM_LIMIT_DEFINITION_MODE_DIRECTION_HIGHLOW	0x2
#define LM_LIMIT_DEFINITION_MODE_DIRECTION_STATE	0x4 /* When the state was reached */

#define LM_LIMIT_SET_MODE_ACTIVE				0x1

#define LM_LIMIT_MAX_SETS		4
#define LM_LIMIT_MAX_DEVICES	4

typedef struct lm_limit_definition
{
	uint8_t		mode; /* Set via LM_LIMIT_DEFINITION_MODE_* */
	uint16_t	limit_value;		/* All borders fits into 16bit */
	uint8_t		tick_deadband;
	uint8_t		tick_counter;	
} lm_limit_definition_t;

typedef struct lm_limit_set
{
	uint16_t id;
	uint8_t mode;
	lm_limit_definition_t	devices[LM_LIMIT_MAX_DEVICES];
	uint16_t (*getvalue)(void); /* Returns the limit value */
	uint16_t cycles; /* in seconds */
	uint16_t cycleCounter;
} lm_limit_set_t;

PROCESS_NAME(limit_monitoring_process);

inline void lm_setActive(lm_limit_definition_t *definition, uint8_t active);
inline void lm_setDirection(lm_limit_definition_t *definition, uint8_t highlow);

inline uint8_t lm_set_isActive(lm_limit_set_t *set);
inline uint8_t lm_definition_isActive(lm_limit_set_t *definition);

inline void lm_set_setActive(lm_limit_set_t *set, uint8_t active);

#endif /* METERING_H_ */
