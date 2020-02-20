/*
 * Fan.h
 *
 *  Created on: 18 Feb 2020
 *      Author: Joshua
 */

#ifndef FAN_H_
#define FAN_H_

#include "ModbusMaster.h"
#include "ModbusRegister.h"
#include <cstdio>

/* Defined in main */
void delayMilliseconds(uint64_t ms);

class Fan {
public:
	Fan();
	virtual ~Fan();

	bool setFrequency(uint16_t freq);
	uint16_t getFrequency();


	void abbModBusTest();
	void modBusTest();

private:
	ModbusMaster node;
	static constexpr uint16_t min_freq 	= 1400; // Anything below 1400 will not power up the fan.
	static constexpr uint16_t max_freq 	= 20000;
	static constexpr uint16_t delay 	= 500;
};

#endif /* FAN_H_ */
