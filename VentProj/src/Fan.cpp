/*
 * Fan.cpp
 *
 *  Created on: 18 Feb 2020
 *      Authors: Joshua Waugh & Visa Harvey
 */

#include "Fan.h"

Fan::Fan() : node{ 2 } {
	ModbusRegister ControlWord { node, 0 };
	ModbusRegister StatusWord  { node, 3 };
	ModbusRegister OutputFrequency { node, 102 };
	ModbusRegister Current { node, 103 };

	/* Reset state machine */
	ControlWord = 0x406; // (CW 0b0110)
	Sleep(kDelay);
	OutputFrequency = 0;
	Sleep(kDelay);
	Current = 0;

	do {
		Sleep(kDelay);
	} while (!(static_cast<int>(StatusWord) & ModbusRegister::ReadyToSwitchOn));

	Sleep(kDelay);
	ControlWord = 0x407; // (CW 0b0111)

	do {
		Sleep(kDelay);
	} while (!(static_cast<int>(StatusWord) & (ModbusRegister::ExternalControlEnabled | ModbusRegister::ReadyToOperate)));

	Sleep(kDelay);
	ControlWord = 0x40F; // (CW Bit3=1)

	do {
		Sleep(kDelay);
	} while (!(static_cast<int>(StatusWord) & ModbusRegister::OperationEnabled));

	Sleep(kDelay);
	ControlWord = 0x42F; // (CW Bit5=1)

	Sleep(kDelay);
	ControlWord = 0x46F; // (CW Bit6=1)

	Sleep(kDelay);
	ControlWord = 0x47F; // (CW Bit7=1)

	do {
		Sleep(kDelay);
	} while (!(static_cast<int>(StatusWord) & ModbusRegister::Operating));
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
