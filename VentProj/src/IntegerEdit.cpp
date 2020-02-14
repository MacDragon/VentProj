/*
 * IntegerEdit.cpp
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#include "IntegerEdit.h"
#include <cstdio>

IntegerEdit::IntegerEdit(LiquidCrystal *lcd_, std::string editTitle,  int lower, int upper, int step)
: lcd(lcd_), title(editTitle), lower(lower), upper(upper), step(step) {
	value = lower;
	edit = lower;
	if ( step < 1 ) step = 1; // increment of 0 is useless, and keeps positive
	focus = false;
	callback = NULL;
}

IntegerEdit::~IntegerEdit() {
}

void IntegerEdit::increment() {
	if ( edit+step <= upper )
		edit+=step;
}

void IntegerEdit::decrement() {
	if ( edit-step >= lower )
	edit-=step;
}

void IntegerEdit::accept() {
	save();
}

void IntegerEdit::cancel() {
	edit = value;
}


void IntegerEdit::setFocus(bool focus) {
	this->focus = focus;
}

bool IntegerEdit::getFocus() {
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
	if ( edit != value )
	{
		value = edit;
		if ( callback != NULL )
			callback();
	}
	// todo: save current value for example to EEPROM for permanent storage
}


int IntegerEdit::getValue() {
	return value;
}
void IntegerEdit::setValue(int value) {
	if ( value >= lower && value <= upper )
	{
		edit = value;
		save();
	}
}

void IntegerEdit::setCallback( void (*callbackfunc)() )
{
	if (callbackfunc != NULL ) callback = callbackfunc;
}
