/*
 * I2C.h
 *
 *  Created on: 21.2.2016
 *      Author: krl
 */

#ifndef I2C_H_
#define I2C_H_

#include "chip.h"

class I2C {
public:
	I2C(unsigned int const device_number = 0, unsigned int const speed = 10000, unsigned int const clock_divider = 40, unsigned int const i2c_mode = IOCON_SFI2C_EN);
	virtual ~I2C();
	bool write(uint8_t const devAddr, uint8_t const * const txBuffPtr, uint16_t const txSize) const;
	bool read(uint8_t const devAddr, uint8_t* const rxBuffPtr, uint16_t const rxSize) const;
	bool transaction(uint8_t const devAddr, uint8_t const * const txBuffPtr, uint16_t const txSize, uint8_t* const rxBuffPtr, uint16_t const rxSize) const;
private:
	LPC_I2C_T *device;
	static uint32_t I2CM_XferBlocking(LPC_I2C_T* const pI2C, I2CM_XFER_T* const xfer);
};

#endif /* I2C_H_ */
