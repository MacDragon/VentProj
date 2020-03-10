/*
 * Imutex.cpp
 *
 *  Created on: 2 Feb 2020
 *      Author: Joshua
 */

#include "Imutex.h"

void Imutex::lock() {
	enable = (__get_PRIMASK() & 1) == 0;
	__disable_irq();
}

void Imutex::unlock() {
	if (enable)
		__enable_irq();
}


