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
	delayMilliseconds(1000);
	node.writeSingleRegister(0, 0x047F);
	/* Note:
	 * we should have a startup state machine that check converter status and acts per current status
	 * but we take the easy way out and just wait a while and hope that everything goes well
	 */
}

Fan::~Fan() {
	// TODO Auto-generated destructor stub
}

bool Fan::setFrequency(uint16_t freq) {
	int  ctr{ 0 }, result{ 0 };
	bool atSetpoint{ false };
	ModbusRegister StatusWord(&node, 3);

	//if (freq > max_freq)
		//freq = max_freq;
	//else if (freq < min_freq)
		//freq = min_freq;

	node.writeSingleRegister(1, freq);

	do {
		delayMilliseconds(delay); // Systick priority is really low and I didn't feel like messing with it.
		result = StatusWord;
		atSetpoint = (result >= 0 && result & 0x0100);
	} while(++ctr < 20 && !atSetpoint);

	return atSetpoint;
}

uint16_t Fan::getFrequency() {
	ModbusRegister OutputFrequency{ &node, 102 };
	return OutputFrequency;
}

void Fan::abbModBusTest() {
	uint8_t i{ 0 };
	constexpr uint16_t fa[] { 1000, 2000, 3000, 3500, 4000, 5000, 7000, 8000, 10000, 15000, 20000, 9000, 8000, 7000, 6000, 5000, 4000, 3000, 2000, 1000 };
	ModbusRegister OutputFrequency{ &node, 102 }, Current{ &node, 103 };
	while (1) {
		/* Just print the value without checking if we got a -1 */
		// Seems like 20000 f == 100 I, so 5 I == 1000 F
		printf("F=%4d, I=%4d\n", static_cast<int>(OutputFrequency), static_cast<int>(Current));
		delayMilliseconds(3000);
		if(i >= 20)
			i=0;

		setFrequency(fa[i++]); // frequency is scaled: 20000 = 50 Hz, 0 = 0 Hz, linear scale 400 units/Hz
	}
}

void Fan::modBusTest() {
	while (1) {
		static uint16_t i;
		uint8_t j, result;
		uint16_t data[6];

		for(j = 0; j < 6; j++)
			/* Set word(j) of TX buffer to least-significant word of counter (bits 15..0) */
			node.setTransmitBuffer(j, ++i);

		/* Slave: write TX buffer to (6) 16-bit registers starting at register 0 */
		result = node.writeMultipleRegisters(0, j);

		/* Slave: read (6) 16-bit registers starting at register 2 to RX buffer */
		result = node.readHoldingRegisters(2, 6);

		/* Do something with data if read is successful */
		if (result == node.ku8MBSuccess) {
			for (j = 0; j < 6; j++)
				data[j] = node.getResponseBuffer(j);
			printf("%6d, %6d, %6d, %6d, %6d, %6d\n", data[0], data[1], data[2], data[3], data[4], data[5]);
		}
		delayMilliseconds(1000);
	}

}

