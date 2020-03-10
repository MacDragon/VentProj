/*
 * Fan.h
 *
 *  Created on: 18 Feb 2020
 *      Author: Joshua
 */

#ifndef FAN_H_
#define FAN_H_

#include "ModbusRegister.h"
#include "ModbusMaster.h"
#include <cstdio>

void Sleep(unsigned int ms);

class Fan {
public:
	Fan();

	bool 	setFrequency(uint16_t freq);
	int16_t getFrequency();

	static constexpr int16_t kFanError 	= -1;

private:
	ModbusMaster node;
	static constexpr uint16_t kMinFreq 	= 0; // Not used, probably not accurate either.
	static constexpr uint16_t kMaxFreq 	= 20000;
	static constexpr uint16_t kDelay 	= 50;
};

#endif /* FAN_H_ */
