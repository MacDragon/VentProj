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


class ModeEdit: public PropertyEdit {
public:
	/* Lazy state machine. Instances of this class are each associated with a Mode
	 * and will modify the state of the class (fanMode) when the accept() function
	 * is called. */
	enum Mode { Manual, Automatic, Startup };
	static std::atomic<Mode> fanMode;

	ModeEdit(LiquidCrystal* lcd, std::string const& title, std::string const& subTitle, int const lowerLimit, int const upperLimit, Mode const mode = Manual);
	virtual ~ModeEdit();
	void increment();
	void decrement();
	void accept();
	void cancel();
	void setFocus(bool focus);
	bool getFocus() const;
	void display();

	int  getValue() const;
	void setValue(int const value);


private:
	LiquidCrystal* lcd;
	std::string const title, subTitle;
	int const lowerLimit, upperLimit;
	int value, edit;
	bool focus;
	Mode const mode;
};



#endif /* INTEGEREDIT_H_ */
