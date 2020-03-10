/*
 * Fan.cpp
 *
 *  Created on: 18 Feb 2020
 *      Author: Joshua
 */

#include "Fan.h"

Fan::Fan() : node{ 2 } {
	ModbusRegister ControlWord { node, 0 };
	ModbusRegister StatusWord { node, 3 };

	/* Not stepping through the state machine laid out
	 * in the documentation. If it fails and takes too
	 * long, the watchdog timer will cause a reset.
	 */
	ControlWord = 0x0406;
	while (!(static_cast<int>(StatusWord) & 0x001))
		Sleep(10);

	ControlWord = 0x047F;
	while (!(static_cast<int>(StatusWord) & 0x100))
		Sleep(10);
}

bool Fan::setFrequency(uint16_t freq) {
	int  ctr { 0 };
	bool atSetpoint { false };
	ModbusRegister Frequency { node, 1 };
	ModbusRegister StatusWord { node, 3 };

	freq *= 200; /* Convert from percentage to raw value */

	if (freq > kMaxFreq)
		freq = kMaxFreq;
	else if (freq < kMinFreq)
		freq = kMinFreq;

	Frequency = freq;

	do {
		Sleep(kDelay);
		auto result = static_cast<int>(StatusWord);
		atSetpoint = (result >= 0 && result & 0x0100);
	} while (++ctr < 20 && !atSetpoint);

	return atSetpoint;
}

int16_t Fan::getFrequency() {
	auto outputFrequency = static_cast<int16_t>(ModbusRegister{ node, 102 });
	if (outputFrequency >= 0)
		outputFrequency /= 200; // convert from raw value to percentage
	return outputFrequency;
}
