/*
 * PressureSensor.h
 *
 *  Created on: 4 Mar 2020
 *      Author: Joshua
 */

#ifndef PRESSURESENSOR_H_
#define PRESSURESENSOR_H_

#include "Subject.h"
#include <vector>
#include "I2C.h"

void Sleep(unsigned int ms);

class PressureSensor : public Subject {
public:
	PressureSensor(const I2C& i2c);
	virtual ~PressureSensor();
	std::pair<bool, int16_t> getPressure();

	void attach(PropertyEdit* observer);
	void notify();

private:
	I2C i2c;
	std::vector<PropertyEdit*> observers;
	int16_t pressure;
	static constexpr uint8_t sensorReadCMD { 0xF1 };
	static constexpr uint8_t i2c_pressure_address { 0x40 };
	uint8_t sensorData[3];
};

#endif /* PRESSURESENSOR_H_ */
