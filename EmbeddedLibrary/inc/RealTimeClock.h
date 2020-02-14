/*
 * RealTimeClock.h
 *
 *  Created on: 29 Jan 2020
 *      Author: visa
 */

#ifndef REALTIMECLOCK_H_
#define REALTIMECLOCK_H_

#include <mutex>
#include "Imutex.h"

class RealTimeClock {
	public:
// You must also provide a copnstructor variant with systick rate and current time
	RealTimeClock(const int ticksPerSecond, tm now = {35,58,23});
	void gettime(tm *now);
	void tick();
private:
	Imutex guard;
	int ticks; // only set at construction, not volatile.
	int tickspersec;
	volatile int hour;
	volatile int min;
	volatile int sec;
};

#endif /* REALTIMECLOCK_H_ */
