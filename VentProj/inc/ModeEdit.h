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

struct DisplayCfg {

};

class ModeEdit: public PropertyEdit {
public:
	enum Mode { Manual, Automatic };
	static std::atomic<Mode> fanMode;

	ModeEdit(Subject& subject, LiquidCrystal& lcd, int const lowerLimit, int const upperLimit, Mode const mode);
	virtual ~ModeEdit();

	/* Menu interface implementation */
	void increment();
	void decrement();
	void change(int amount);
	void accept();
	void cancel();
	void setFocus(bool focus);
	bool getFocus() const;
	void display();

	/* Observer pattern interface implementation */
	void update(int value);

	int  getValue() const;
	void setValue(int value);

private:
	Subject& subject;
	LiquidCrystal& lcd;
	BarGraph bar;

	std::string title, subTitle, editUnit, dispTitle, dispUnit;
	int const lowerLimit, upperLimit;
	int value, edit, observedValue;
	bool focus;
	Mode const mode;
	unsigned int ErrTime;
	static constexpr unsigned int error_threshold = 5000; // 5 seconds
};
#endif /* INTEGEREDIT_H_ */
