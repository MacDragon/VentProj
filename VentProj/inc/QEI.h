/*
 * QEI.h
 *
 *  Created on: 21.2.2016
 *      Author: krl
 */

#ifndef QEI_H_
#define QEI_H_

#include "chip.h"
#include "DigitalIoPin.h"
#include "LpcUart.h"

class QEI {
public:
	QEI( LpcPinMap A, LpcPinMap B, uint16_t maxvel = 5);
	virtual ~QEI();
	int read();
private:
	bool ignore;
	uint16_t maxvel;
	uint8_t lastdir;

};

#endif /* QEI_H_ */
