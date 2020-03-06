/*
 * PressureSensor.cpp
 *
 *  Created on: 4 Mar 2020
 *      Author: Joshua
 */

#include "PressureSensor.h"

constexpr uint8_t PressureSensor::sensorReadCMD;
constexpr uint8_t PressureSensor::i2c_pressure_address;

PressureSensor::PressureSensor(const I2C& i2c) : i2c{ i2c } { /* Empty */ }

PressureSensor::~PressureSensor() { /* Empty */ }

std::pair<bool, int16_t> PressureSensor::getPressure() {
	if (i2c.transaction(i2c_pressure_address, &sensorReadCMD, 1, sensorData, 3)) {
		pressure = static_cast<int16_t>(sensorData[0] << 8 | sensorData[1]) / 240 * 0.95;
		notify();
		return { true, pressure };
	} else
		return { false, 0 };
}

void PressureSensor::attach(PropertyEdit* observer) {
	observers.push_back(observer);
};

void PressureSensor::notify() {
	for (const auto& observer : observers) {
		observer->update(pressure);
	}
};
