/*
 * Fan.cpp
 *
 *  Created on: 18 Feb 2020
 *      Author: Joshua
 */

#include "Fan.h"

Fan::Fan() : node(2) {
	ModbusRegister StatusWord(&node, 3);
	node.writeSingleRegister(0, 0x0406); // Prepare for starting
	Sleep(1000);
	node.writeSingleRegister(0, 0x047F);

	/* Note:
	 * we should have a startup state machine that check converter status and acts per current status
	 * but we take the easy way out and just wait a while and hope that everything goes well
	 */
	setFrequency(0);
}

Fan::~Fan() {
	// TODO Auto-generated destructor stub
}

bool Fan::setFrequency(uint16_t freq) {
	int  ctr{ 0 }, result{ 0 };
	bool atSetpoint{ false };
	ModbusRegister StatusWord(&node, 3);

	if (freq > max_freq)
		freq = max_freq;

	node.writeSingleRegister(1, freq);

	do {
		Sleep(delay); // Systick priority is really low and I didn't feel like messing with it.
		result = StatusWord;
		atSetpoint = (result >= 0 && result & 0x0100);
	} while(++ctr < 20 && !atSetpoint);

	return atSetpoint;
}

uint16_t Fan::getFrequency() {
	ModbusRegister OutputFrequency{ &node, 102 };
	return OutputFrequency;
}
