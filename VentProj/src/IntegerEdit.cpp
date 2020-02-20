/*
 * IntegerEdit.cpp
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#include "IntegerEdit.h"
#include <cstdio>

IntegerEdit::IntegerEdit(LiquidCrystal *lcd_, std::string const& editTitle, int const lowerLimit, int const upperLimit, int const stepSize) :
lcd(lcd_), title(editTitle), lowerLimit{ lowerLimit }, upperLimit{ upperLimit }, stepSize{ stepSize }, value{ lowerLimit }, edit{ lowerLimit }, focus{ false } {}

IntegerEdit::~IntegerEdit() {}

void IntegerEdit::increment() {
	if(edit < upperLimit)
		edit++;
}

void IntegerEdit::decrement() {
	if(edit > lowerLimit)
		edit--;
}

void IntegerEdit::accept() {
	save();
}

void IntegerEdit::cancel() {
	edit = value;
}

void IntegerEdit::setFocus(bool const focus) {
	this->focus = focus;
}

bool IntegerEdit::getFocus() const {
	return this->focus;
}

void IntegerEdit::display() {
	lcd->clear();
	lcd->setCursor(0,0);
	lcd->print(title);
	lcd->setCursor(0,1);
	char s[17];
	if(focus) {
		snprintf(s, 17, "     [%4d]     ", edit);
	}
	else {
		snprintf(s, 17, "      %4d      ", edit);
	}
	lcd->print(s);
}


void IntegerEdit::save() {
	// set current value to be same as edit value
	value = edit;
	if(callback != nullptr)
		callback(*this);
	// todo: save current value for example to EEPROM for permanent storage
}

int IntegerEdit::getValue() const {
	return value;
}
void IntegerEdit::setValue(int const value) {
	edit = value;
	save();
}

void IntegerEdit::setCallback(void (*callback)(const IntegerEdit&)) {
	this->callback = callback;
}


