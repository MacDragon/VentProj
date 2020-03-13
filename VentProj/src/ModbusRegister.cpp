/*
 * ModbusRegister.cpp
 *
 *  Created on: 13.2.2019
 *      Author: keijo
 */

#include "ModbusRegister.h"

ModbusRegister::operator int() {
	auto result = m.readHoldingRegisters(addr, 1);
	if (result == m.ku8MBSuccess)
		return m.getResponseBuffer(0);
	return -1;
}

const ModbusRegister& ModbusRegister::operator=(int value) {
	m.writeSingleRegister(addr, value);
	return *this;
}
