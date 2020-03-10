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
	constexpr ModbusRegister(ModbusMaster& master, uint32_t const address) : m{ master }, addr{ address } {}
	ModbusRegister(const ModbusRegister&) = delete;
	operator int();
	const ModbusRegister& operator=(int value);

private:
	ModbusMaster& m;
	uint32_t const addr;
};

#endif /* MODBUSREGISTER_H_ */
