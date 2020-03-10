/*
 * QEI.h
 *
 *  Created on: 21.2.2016
 *      Author: krl
 */

#ifndef QEI_H_
#define QEI_H_

#include "DigitalIoPin.h"
#include "LpcUart.h"
#include "chip.h"

class QEI {
public:
	QEI(LpcPinMap A, LpcPinMap B, uint16_t maxvel = 5);
	int read();

private:
	bool ignore;
	uint16_t const maxvel;
	uint8_t lastdir;

};

#endif /* QEI_H_ */
