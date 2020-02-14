/*
 * DecimalEdit.h
 *
 *  Created on:
 *      Author: vh
 */

#ifndef DECIMALEDIT_H_
#define DECIMALEDIT_H_

#include "PropertyEdit.h"
#include "LiquidCrystal.h"
#include <string>

class DecimalEdit: public PropertyEdit {
public:
	DecimalEdit(LiquidCrystal *lcd_, std::string editTitle, float lower = 0, float upper = 100, float step = 1);
	virtual ~DecimalEdit();
	void increment();
	void decrement();
	void accept();
	void cancel();
	void setFocus(bool focus);
	bool getFocus();
	void display();
	float getValue();
	void setValue(float value);
	void setCallback( void (*callback)() );
private:
	void save();
	void displayEditValue();
	LiquidCrystal *lcd;
	std::string title;
	int value;
	int edit;
	bool focus;
	int lower;
	int upper;
	int step;
	void (*callback)();
};

#endif
