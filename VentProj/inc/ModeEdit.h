/*
 * IntegerEdit.h
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#ifndef INTEGEREDIT_H_
#define INTEGEREDIT_H_

#include "LiquidCrystal.h"
#include "PropertyEdit.h"
#include "BarGraph.h"
#include "Subject.h"
#include <string>
#include <atomic>

uint32_t millis();

class ModeEdit: public PropertyEdit {
public:
	enum Mode { Manual, Automatic };
	static std::atomic<Mode> fanMode;

	ModeEdit(LiquidCrystal& lcd, int const lowerLimit, int const upperLimit, Mode const mode);

	/* Menu interface implementation */
	void increment();
	void decrement();
	void change(int amount);
	void accept();
	void cancel();
	void setFocus(bool focus);
	bool getFocus() const;
	void display();
	int  getValue() const;
	void setValue(int value);

	/* Observer pattern */
	void observe(Subject& subject);
	void update(int value);

private:
	LiquidCrystal& lcd;
	BarGraph bar;

	std::string title, unit;
	int const lowerLimit, upperLimit;
	std::atomic<int> value, edit;
	int observedValue;
	bool focus;
	Mode const mode;
	unsigned int ErrTime;

	static constexpr char kSubtitle[] = "Cur Pres";
	static constexpr char kSubUnit[]  = "Pa";
	static constexpr unsigned int kErrorThreshold = 10000;
};
#endif /* INTEGEREDIT_H_ */
