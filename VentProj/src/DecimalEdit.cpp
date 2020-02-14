/*
 * IntegerEdit.cpp
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#include "DecimalEdit.h"
#include <cstdio>

DecimalEdit::DecimalEdit(LiquidCrystal *lcd_, std::string editTitle, float lower, float upper, float step)
: lcd(lcd_), title(editTitle), lower(lower*10), upper(upper*10), step(step*10) {
	value = this->lower;
	edit = this->lower;
	if ( step < 1 ) step = 10; // increment of 0 is useless, and keeps positive
	focus = false;
	callback = NULL;
}

DecimalEdit::~DecimalEdit() {
}

void DecimalEdit::display() {
	lcd->clear();
	lcd->setCursor(0,0);
	lcd->print(title);
	lcd->setCursor(0,1);
	char s[17];
	if(focus) {
		snprintf(s, 17, "     [%6.1f]     ", edit/10.0);
	}
	else {
		snprintf(s, 17, "      %6.1f      ", edit/10.0);
	}
	lcd->print(s);
}

float DecimalEdit::getValue() {
	return value/10.0;
}

void DecimalEdit::setValue(float value) {
	value = value*10;
	if ( value >= lower && value <= upper )
	{
		edit = value;
		save();
	}
}

void DecimalEdit::increment() {
	if ( edit+step <= upper )
		edit+=step;
}

void DecimalEdit::decrement() {
	if ( edit-step >= lower )
	edit-=step;
}

void DecimalEdit::accept() {
	save();
}

void DecimalEdit::cancel() {
	edit = value;
}


void DecimalEdit::setFocus(bool focus) {
	this->focus = focus;
}

bool DecimalEdit::getFocus() {
	return this->focus;
}


void DecimalEdit::save() {
	// set current value to be same as edit value
	if ( edit != value )
	{
		value = edit;
		if ( callback != NULL )
			callback();
	}
	// todo: save current value for example to EEPROM for permanent storage
}


void DecimalEdit::setCallback( void (*callbackfunc)() )
{
	if (callbackfunc != NULL ) callback = callbackfunc;
}
