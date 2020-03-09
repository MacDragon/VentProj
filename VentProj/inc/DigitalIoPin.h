/*
 * DigitalIoPin.h
 *
 *  Created on: 15 Jan 2020
 *      Author: Joshua
 */

#ifndef DIGITALIOPIN_H_
#define DIGITALIOPIN_H_
#include "chip.h"

class DigitalIoPin {
public:
	DigitalIoPin(int const port, int const pin, bool const input, bool const pullup, bool const invert) :
	port { port }, pin { pin }, input { input }, invert { invert }
	{
		LPC_IOCON->PIO[port][pin] = (1U + pullup) << 3 | 1U << 7 | invert << 6;
		LPC_GPIO->DIR[port] = input ? LPC_GPIO->DIR[port] & ~(1UL << pin) : LPC_GPIO->DIR[port] | 1UL << pin;
	}

	virtual ~DigitalIoPin() { /* Empty */ }

	bool read() const {
		return static_cast<bool>(LPC_GPIO->B[port][pin]);
	}

	void write(const bool value) const {
		LPC_GPIO->B[port][pin] = invert ? !value : value;
	}

	void toggle() const {
		LPC_GPIO->B[port][pin] = invert ? read() : !read();
	}

private:
	int const port, pin;
	bool const input, invert;
};

#endif /* DIGITALIOPIN_H_ */
