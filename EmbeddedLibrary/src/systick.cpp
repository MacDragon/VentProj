/*
 * systick.cpp
 *
 *  Created on: 17 Jan 2020
 *      Author: visa
 */

#include "systick.h"
#include "Imutex.h"

static volatile std::atomic_int counter;

static RealTimeClock *rtc;

static LEDPin *activeled;

uint32_t timebase = 0;

static volatile std::atomic_uint32_t blinkr;
static volatile std::atomic_uint32_t ledon;
static volatile std::atomic_uint32_t ledoff;

uint32_t ledcount = 0;

static volatile std::atomic_uint32_t led;

void SetRTC(RealTimeClock& rtcobj) {
	rtc = &rtcobj;
}


void SetBlinkrate(uint32_t blinkrate)
{
	blinkr = blinkrate;
}

void SetLEDtime(uint32_t on, uint32_t off)
{
	ledon = on;
	ledoff = off;
}


void SetLED(LEDPin &led, uint32_t on, uint32_t off) {
	static Imutex mutex;
	mutex.lock();
	if ( activeled != &led )
	{
		if ( activeled != NULL ) activeled->write(false);
		activeled = &led;
		if ( on == 0 && off == 0 ) activeled = NULL;
	}
	ledon = on;
	ledoff = off;
	mutex.unlock();
}


#ifdef OLDLED
void SetLED(LEDPin &led, uint32_t blinkrate = 0) {
	static Imutex mutex;
	mutex.lock();
	if ( activeled != &led )
	{
		if ( activeled != NULL ) activeled->write(false);
		activeled = &led;
	}
	blinkr = blinkrate;
	mutex.unlock();
}

#endif


#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief	Handle interrupt from SysTick timer
* @return	Nothing
*/
void SysTick_Handler(void) {
	if(rtc != NULL) rtc->tick();
	if ( activeled != NULL ) {
		if ( activeled->read() == true && ledcount > ledon )
		{
			if ( ledoff >0 )
				activeled->write(false);
			ledcount=0;
		}

		if ( activeled->read() == false&& ledcount > ledoff )
		{
			if ( ledon > 0 )
				activeled->write(true);
			ledcount=0;
		}

		ledcount++;
	}
	if(counter > 0) counter--;
	timebase++;
}


#ifdef OLDLED
/**
* @brief	Handle interrupt from SysTick timer
* @return	Nothing
*/
void SysTick_Handler(void) {
	if(rtc != NULL) rtc->tick();
	if ( activeled != NULL ) {
		if ( ledcount > blinkr )
		{
			activeled->toggle();
			ledcount=0;
		} else ledcount++;
	}
	if(counter > 0) counter--;
	timebase++;
}
#endif

#ifdef __cplusplus
}
#endif

void Sleep(int ms)
{
	counter = ms;
	while(counter > 0) {
		__WFI();
	}
}

/*
 *
Modify delayMicroseconds function to calculate RIT compare value from Chip_Clock_GetSystemClockRate()
and the requested wait time. Note that the compare value is a 64-bit unsigned integer
so you must use 64- bit variables for calculations. Use uint64_t for 64-bit integers.
 *
 */

void delayMicroseconds(unsigned int us)
{
// calculate compare value
	uint64_t end = (SystemCoreClock)/1000000*us;
// or uint64_t end = Chip_Clock_GetSystemClockRate()/1000000*us

// disable RIT – compare value may only be changed when RIT is disabled
	Chip_RIT_Disable(LPC_RITIMER);
// set compare value to RIT

	Chip_RIT_SetCompareValue(LPC_RITIMER, end);
// clear RIT counter (so that counting starts from zero)
	Chip_RIT_SetCounter(LPC_RITIMER, 0);

// enable RIT
	Chip_RIT_Enable(LPC_RITIMER);

// wait until RIT Int flag is set

	while( !Chip_RIT_GetIntStatus(LPC_RITIMER) ) {
	//	__WFI();
	}
// disable RIT
	Chip_RIT_Disable(LPC_RITIMER);
// clear RIT Int flag

	Chip_RIT_ClearIntStatus(LPC_RITIMER);

}

void Sleepus(int us)
{
	uint32_t end = (SystemCoreClock)/1000000*us;
	uint32_t start = DWT->CYCCNT;
	uint32_t current = DWT->CYCCNT;
	do {

	 current = DWT->CYCCNT;

	 } while((current - start) < end);
}


void SysTickSetup()
{

	blinkr = 0;
	ledcount = 0;

	uint32_t sysTickRate;

	/* The sysTick counter only has 24 bits of precision, so it will
	   overflow quickly with a fast core clock. You can alter the
	   sysTick divider to generate slower sysTick clock rates. */
	Chip_Clock_SetSysTickClockDiv(1);

	/* A SysTick divider is present that scales the sysTick rate down
	   from the core clock. Using the SystemCoreClock variable as a
	   rate reference for the SysTick_Config() function won't work,
	   so get the sysTick rate by calling Chip_Clock_GetSysTickClockRate() */
	sysTickRate = Chip_Clock_GetSysTickClockRate();

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(sysTickRate / TICKRATE_HZ1);

	CoreDebug->DEMCR |= 1 << 24; // enable DWT for high accuracy read.
	DWT->CTRL |= 1;
}

