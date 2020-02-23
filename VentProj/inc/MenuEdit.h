/*
 * MenuEdit.h
 *
 *  Created on: 21 Feb 2020
 *      Author: Joshua
 */

#ifndef MENUEDIT_H_
#define MENUEDIT_H_

#include <string>
#include "PropertyEdit.h"
#include "LiquidCrystal.h"

/* This class basically only has a display() function but it inherits from an interface
 * so all of its functions had to be implemented. */

uint32_t millis();

class MenuEdit : public PropertyEdit {
public:
	MenuEdit(LiquidCrystal* lcd, std::string const& row1Text, std::string const& row2Text, std::string const& errText = "");
	virtual ~MenuEdit();
	void increment();
	void decrement();
	void accept();
	void cancel();
	void setFocus(bool focus);
	bool getFocus() const;
	void display();

	void setRow1Values(int const currentValue, int const targetValue);
	void setRow2Values(int const currentValue, int const targetValue);

private:
	LiquidCrystal* lcd;
	std::string const row1Text, row2Text, errText;
	int row1value, row2value;
	unsigned int row1ErrCount, row2ErrCount;

	static constexpr unsigned int error_threshold = 5000; // 5 seconds
};

#endif /* MENUEDIT_H_ */
