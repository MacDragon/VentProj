/*
 * DigitalIoPin.h
 *
 *  Created on: 15 Jan 2020
 *      Author: Joshua
 */

#ifndef DIGITALIOPIN_H_
#define DIGITALIOPIN_H_
#include "chip.h"
#include "board.h"

class DigitalIoPin {
private:
	int const port, pin;
	bool const input, invert;

public:
	DigitalIoPin(int const port, int const pin, bool const input, bool const pullup, bool const invert);
	virtual ~DigitalIoPin();

	constexpr bool read() const {
		return static_cast<bool>(LPC_GPIO->B[port][pin]);
	}

	void write(const bool value) const {
		LPC_GPIO->B[port][pin] = invert ? !value : value;
	}

	void toggle() const {
		LPC_GPIO->B[port][pin] = invert ? read() : !read();
	}
};

#endif /* DIGITALIOPIN_H_ */
