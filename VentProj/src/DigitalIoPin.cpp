/*
 * DigitalIoPin.cpp
 *
 *  Created on: 15 Jan 2020
 *      Author: Joshua
 */

#include "DigitalIoPin.h"

DigitalIoPin::DigitalIoPin(int const port, int const pin, bool const input, bool const pullup, bool const invert) :
port { port }, pin { pin }, input { input }, invert { invert }
{
	LPC_IOCON->PIO[port][pin] = (1U + pullup) << 3 | 1U << 7 | invert << 6;

	LPC_GPIO->DIR[port] = input ? LPC_GPIO->DIR[port] & ~(1UL << pin) : LPC_GPIO->DIR[port] | 1UL << pin;
}


DigitalIoPin::~DigitalIoPin() {
	// TODO Auto-generated destructor stub
}

