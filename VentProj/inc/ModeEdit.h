/*
 * IntegerEdit.h
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#ifndef INTEGEREDIT_H_
#define INTEGEREDIT_H_

#include "PropertyEdit.h"
#include "LiquidCrystal.h"
#include <string>
#include <atomic>
#include "BarGraph.h"

uint32_t millis();

class ModeEdit: public PropertyEdit {
public:
	/* Lazy state machine. Instances of this class are each associated with a Mode
	 * and will modify the state of the class (fanMode) when the accept() function
	 * is called. */
	enum Mode { Manual, Automatic, Startup };
	static std::atomic<Mode> fanMode;

	ModeEdit(LiquidCrystal* lcd, int const lowerLimit, int const upperLimit, Mode const mode);
	virtual ~ModeEdit();
	void increment();
	void decrement();
	void change(int amount);
	void accept();
	void cancel();
	void setFocus(bool focus);
	bool getFocus() const;
	void display();

	int  getValue() const;
	void setValue(int const value);
	void setDispValue2(const int value);



private:
	LiquidCrystal* lcd;
	BarGraph bar;

	std::string title, subTitle, editUnit;
	std::string dispTitle, dispUnit;
	int const lowerLimit, upperLimit;
	int value, edit, value2;
	bool focus;
	Mode const mode;
	unsigned int ErrTime;
	static constexpr unsigned int error_threshold = 5000; // 5 seconds
};



#endif /* INTEGEREDIT_H_ */
