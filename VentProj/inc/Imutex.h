/*
 * Imutex.h
 *
 *  Created on: 2 Feb 2020
 *      Author: Joshua
 */

#ifndef IMUTEX_H_
#define IMUTEX_H_

#include "chip.h"
#include <mutex>

class Imutex {
public:
	Imutex();
	~Imutex();
	void lock();
	void unlock();
private:
	bool enable;
};


#endif /* IMUTEX_H_ */
