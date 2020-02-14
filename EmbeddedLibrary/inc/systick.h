/*
 * systick.h
 *
 *  Created on: 17 Jan 2020
 *      Author: visa
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "RealTimeClock.h"
#include "DigIO.h"

#include <atomic>

#define TICKRATE_HZ1 (1000)	/* 1000 ticks per second */

extern uint32_t timebase;

void SetRTC(RealTimeClock& rtcobj);
#ifdef OLDLED
void SetLED(LEDPin &led, uint32_t blinkrate);
#endif
void SetLED(LEDPin &led, uint32_t ledon = 100, uint32_t ledoff = 0);

//void SetLEDs(LEDPin &redled, LEDPin &greenled, LEDPin &blueled);
void SetBlinkrate(uint32_t blinkrate);



void SysTickSetup();

void Sleep(int ms);
void Sleepus( int us);
void delayMicroseconds(unsigned int us);

#endif /* SYSTICK_H_ */
