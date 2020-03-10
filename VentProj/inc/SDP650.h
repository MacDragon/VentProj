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

class SDP650 : public Subject {
public:
	SDP650(I2C&& i2c = I2C{});
	virtual ~SDP650();
	int16_t getPressure();

	void attach(PropertyEdit* observer);
	void notify();

	static constexpr int16_t kI2CError { INT16_MAX };

private:
	I2C i2c;
	int16_t pressure;
	uint8_t sensorData[3];
	std::vector<PropertyEdit*> observers;

	static constexpr uint8_t kSDP650ReadCMD { 0xF1 };
	static constexpr uint8_t kSDP650Address { 0x40 };
	static constexpr uint8_t kScaleFactor   { 240U };
	static constexpr float 	 kAltCorrection { 0.95 };
};

#endif /* PRESSURESENSOR_H_ */
