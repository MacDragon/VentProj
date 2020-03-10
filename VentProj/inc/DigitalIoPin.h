/*
 * DigitalIoPin.h
 *
 *  Created on: 15 Jan 2020
 *      Author: Joshua
 */

#ifndef DIGITALIOPIN_H_
#define DIGITALIOPIN_H_

#include "chip.h"
#include <functional>


enum class Mode { Edge, Level };
enum class Level { Low, High };

class DigitalIoPin {
public:
	DigitalIoPin(int const port, int const pin, bool const input, bool const pullup, bool const invert);

	bool read() const;
	void write(bool const value) const;
	void toggle() const;
	void enableInterrupt(IRQn_Type IRQn, Mode mode, Level level);

private:
	int const port, pin;
	bool const input, invert;
	static bool interrupt_enabled;
	static constexpr IRQn_Type&& IRQn_min { PIN_INT0_IRQn };
	static constexpr IRQn_Type&& IRQn_max { PIN_INT7_IRQn };
};

#endif /* DIGITALIOPIN_H_ */
