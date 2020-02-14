/*
 * DigIO.h
 *
 *  Created on: 17 Jan 2020
 *      Author: visa
 */

#ifndef DIGIO_H_
#define DIGIO_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <stdint.h>


class DigitalIoPin {
public:
	DigitalIoPin(int port, int pin, bool input = true, bool pullup = true, bool invert = false);
	virtual ~DigitalIoPin();
	bool read();
	void write(bool value);
	void toggle();
private:
    uint8_t port;
    uint8_t pin;
    bool input;
    bool state;
    bool invert;
};


class ButtonPin : public DigitalIoPin {
public:
	ButtonPin(int port, int pin, bool invert = true );
	uint32_t ReadWithBounce(); // check if button is currently pressed with simple poll based debouncing.
	bool ReadOnceWithBounce(); // only returns true once for a button press cycle.
	void ResetPress();
	bool Released(); // check if button has been let go since last read.
private:
	int press;
	bool pressed;
	int release;
	bool returnstate;
	uint32_t timepressed;
	uint32_t lastcall;
};

class OutputPin : public DigitalIoPin {
public:
	OutputPin(int port, int pin, bool invert = false);
	void setcounter(uint32_t time);
	void checkcounter();
private:
	uint32_t offms;
	uint32_t starttime;
};


class LEDPin : public OutputPin {
public:
	LEDPin(int port, int pin, bool invert = true);
};


#endif /* DIGIO_H_ */
