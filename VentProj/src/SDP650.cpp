/*
 * PressureSensor.cpp
 *
 *  Created on: 4 Mar 2020
 *      Author: Joshua
 */

#include "SDP650.h"

constexpr uint8_t SDP650::kSDP650ReadCMD;

SDP650::SDP650(I2C&& i2c) : i2c{ i2c } { /* Empty */ }

SDP650::~SDP650() { /* Empty */ }

int16_t SDP650::getPressure() {
	if (i2c.transaction(kSDP650Address, &kSDP650ReadCMD, 1, sensorData, 3))
		pressure = static_cast<int16_t>(sensorData[0] << 8 | sensorData[1]) / kScaleFactor * kAltCorrection;
	else
		pressure = kI2CError;

	notify();
	return pressure;
}

void SDP650::attach(PropertyEdit* observer) {
	observers.push_back(observer);
};

void SDP650::notify() {
	for (const auto& observer : observers)
		observer->update(pressure);
};
