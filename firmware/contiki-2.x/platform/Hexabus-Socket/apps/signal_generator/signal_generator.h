/*
 * signal_generator.h
 *
 *  Created on: 24.08.2011
 *  Author: tobi
 */

#ifndef SIGNAL_GENERATOR_H_
#define SIGNAL_GENERATOR_H_


#include "process.h"
#include <stdint.h>
#include <string.h>


PROCESS_NAME(signal_generator_process);

uint16_t sg_getValue();

#endif /* SIGNAL_GENERATOR_H_ */
