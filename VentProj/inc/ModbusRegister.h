/*
 * ModbusRegister.h
 *
 *  Created on: 13.2.2019
 *      Author: keijo
 */

#ifndef MODBUSREGISTER_H_
#define MODBUSREGISTER_H_

#include "ModbusMaster.h"

class ModbusRegister {
public:
	constexpr ModbusRegister(ModbusMaster& master, int const address) : m{ master }, addr{ address } {}
	ModbusRegister(const ModbusRegister&) = delete;
	virtual ~ModbusRegister();
	operator int();
	const ModbusRegister& operator=(int value);
private:
	ModbusMaster& m;
	int const addr;
};

#endif /* MODBUSREGISTER_H_ */
