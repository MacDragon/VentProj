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
#include <functional>

class IntegerEdit: public PropertyEdit {
public:
	IntegerEdit(LiquidCrystal *lcd_, std::string const& editTitle, int const lowerLimit, int const upperLimit, int const stepSize = 1);
	virtual ~IntegerEdit();
	void increment();
	void decrement();
	void accept();
	void cancel();
	void setFocus(bool focus);
	bool getFocus() const;
	void display();
	int  getValue() const;
	void setValue(int value);

	void setCallback(void (*callback)(const IntegerEdit&));
private:
	void save();
	LiquidCrystal* lcd;
	std::string const title;
	int const lowerLimit, upperLimit, stepSize;
	int value, edit;
	bool focus;


	void (*callback)(const IntegerEdit&);

};

#endif /* INTEGEREDIT_H_ */
