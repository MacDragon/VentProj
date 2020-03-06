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
void Sleep(unsigned int ms);

class Fan {
public:
	Fan();
	virtual ~Fan();

	bool setFrequency(uint16_t freq);
	int16_t getFrequency();

	static constexpr int16_t fan_error 	= -1;

private:
	ModbusMaster node;
	uint16_t freq;
	static constexpr uint16_t min_freq 	= 0; // Not used, probably not accurate either.
	static constexpr uint16_t max_freq 	= 20000;
	static constexpr uint16_t delay 	= 500;
};

#endif /* FAN_H_ */
