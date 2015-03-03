/*
 * rc_outputs.h
 *
 *  Created on: Feb 26, 2015
 *      Author: ficeto
 */
#ifndef RC_OUTPUT_H_
#define RC_OUTPUT_H_

#include "espino.h"

void rcOutputsInit(void);
void rcOutputsWrite(uint8_t chan, uint16_t value);

#endif /* RC_OUTPUT_H_ */
