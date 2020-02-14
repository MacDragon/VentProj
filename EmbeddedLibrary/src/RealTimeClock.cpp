/*
 * RealTimeClock.cpp
 *
 *  Created on: 29 Jan 2020
 *      Author: visa
 */

#include "RealTimeClock.h"
#include "systick.h"
#include <ctime>

RealTimeClock::RealTimeClock(const int ticksPerSecond, tm now) : ticks(0), tickspersec(ticksPerSecond), hour(now.tm_hour), min(now.tm_min), sec(now.tm_sec){
	// TODO Auto-generated constructor stub
}

void RealTimeClock::gettime(tm *now){
	std::lock_guard<Imutex> lock(guard);

	now->tm_year = 2020;
	now->tm_mon = 1;
	now->tm_hour = hour;
	now->tm_min = min;
	now->tm_sec = sec;

}


void RealTimeClock::tick(){
	if (ticks % tickspersec == 1 ) // second passed
	{
		++sec;
		if ( sec > 59 ){
			sec = 0;
			++min;
			if ( min > 59 ) {
				min = 0;
				++ hour;
				if ( hour > 23 ) {
					hour = 0; // no date tracking, just wraps around.
				}
			}
		}


	}
}
