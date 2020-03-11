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
	void 	setFrequency(uint16_t freq);
	int16_t getFrequency();

private:
	ModbusMaster node;
	static constexpr uint16_t kMinFreq 	= 0;
	static constexpr uint16_t kMaxFreq 	= 20000;
	static constexpr uint16_t kDelay 	= 100;
};

#endif /* FAN_H_ */
