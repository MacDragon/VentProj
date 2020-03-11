/*
 * Fan.cpp
 *
 *  Created on: 18 Feb 2020
 *      Author: Joshua
 */

#include "Fan.h"

Fan::Fan() : node{ 2 } {
	ModbusRegister ControlWord { node, 0 };
	ModbusRegister StatusWord  { node, 3 };

	/* Not stepping through the state machine laid out
	 * in the documentation. If it fails and takes too
	 * long, the watchdog timer will cause a reset. */
	ControlWord = 0x0406;
	while (!(static_cast<int>(StatusWord) & 0x001))
		Sleep(10);

	ControlWord = 0x047F;
	while (!(static_cast<int>(StatusWord) & 0x100))
		Sleep(10);

	Sleep(kDelay);
}

void Fan::setFrequency(uint16_t freq) {
	ModbusRegister Frequency { node, 1 };
	ModbusRegister StatusWord { node, 3 };
	int result { 0 };

	freq *= 200; /* Convert from percentage to raw value */

	if (freq > kMaxFreq)
		freq = kMaxFreq;
	else if (freq < kMinFreq)
		freq = kMinFreq;

	Frequency = freq;

	Sleep(kDelay);

	while ((result = static_cast<int>(StatusWord)) < 0 && !(result &0x100)) { /* WWDT will cause reset if this fails for too long */
		Sleep(kDelay);
	}

}

int16_t Fan::getFrequency() {
	ModbusRegister OutputFrequency { node, 102 };
	int16_t result;

	while ((result = static_cast<int16_t>(OutputFrequency)) == -1) { /* WWDT will cause reset if this fails for too long */
		Sleep(kDelay);
	}

	return result / 200; /* Convert from raw value to percentage */
}
