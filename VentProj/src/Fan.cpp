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
	ModbusRegister OutputFrequency { node, 102 };
	ModbusRegister Current { node, 103 };

	/* Not stepping through the state machine laid out
	 * in the documentation. If it fails and takes too
	 * long, the watchdog timer will cause a reset. */
	ControlWord = 0x400;
	OutputFrequency = 0;
	Current = 0;

	Sleep(kDelay);

	ControlWord = 0x0406;

	do {
		Sleep(kDelay);
	} while (!(static_cast<int>(StatusWord) & 0x001));

	ControlWord = 0x047F;

	do {
		Sleep(kDelay);
	} while (!(static_cast<int>(StatusWord) & 0x100));
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

	do {  /* WWDT will cause reset if this fails for too long */
		Sleep(kDelay);
	} while ((result = static_cast<int>(StatusWord)) < 0 && !(result &0x100));

}

int16_t Fan::getFrequency() {
	ModbusRegister OutputFrequency { node, 102 };
	int16_t result;

	do { /* WWDT will cause reset if this fails for too long */
		Sleep(kDelay);
	} while ((result = static_cast<int16_t>(OutputFrequency)) == -1);

	return result / 200; /* Convert from raw value to percentage */
}
