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
	ControlWord = 0x406; // reset

	Sleep(kDelay);

	OutputFrequency = 0;

	Sleep(kDelay);
	Current = 0;

/*	if ( static_cast<int>(StatusWord) & 0x040)){  // if (SW Bit0=1)

	} */

//	ControlWord = 0x0406; // request ready to switch on (CW xxxx xxxx xxxx x110)

	Sleep(kDelay);

	do {
		Sleep(kDelay);
	} while (!(static_cast<int>(StatusWord) & 0x001)); // wait for (SW Bit0=1)

	// ready to switch on.

	ControlWord = 0x407; // request ready to operate (CW xxxx xxxx xxxx x111)

	do {
		Sleep(kDelay);
	} while (!(static_cast<int>(StatusWord) & 0x002)); 	// wait for (SW Bit1=1)

	// ready to operate.

	do {
		Sleep(kDelay);
	} while (!(static_cast<int>(StatusWord) & 0x1000)); // wait for SW Bit12=1 // external control enabled.

	ControlWord = 0x40F; //  // request operation enabled (CW Bit3=1 after SW Bit12=1)

	volatile int status = 0;
	do {
		Sleep(kDelay);
		status = StatusWord;
	} while (!(static_cast<int>(status) & 0x04)); // wait for (SW Bit2=1)

	Sleep(kDelay);

	ControlWord = 0x42F; // request // (CW Bit5=1)

	Sleep(kDelay);

	ControlWord = 0x046F; // (CW Bit6=1)

	Sleep(kDelay);

	ControlWord = 0x047F; // (CW Bit7=1)



	do {
		Sleep(kDelay);
	} while (!(static_cast<int>(StatusWord) & 0x100)); // wait for (SW Bit8=1)
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
