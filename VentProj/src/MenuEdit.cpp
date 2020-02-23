/*
 * PID.cpp
 *
 *  Created on: 19 Feb 2020
 *      Author: Joshua
 */

#include "MenuEdit.h"

MenuEdit::MenuEdit(LiquidCrystal* lcd, std::string const& row1Text, std::string const& row2Text, std::string const& errText) :
lcd{ lcd }, row1Text{ row1Text }, row2Text{ row2Text }, errText{ errText },
row1value{ 0 }, row2value{ 0 }, row1ErrCount{ millis() }, row2ErrCount{ 0 } {
	/* Empty */
}

MenuEdit::~MenuEdit() {
	/* Empty */
}

void MenuEdit::increment() {
	return;
}

void MenuEdit::decrement() {
	return;
}

void MenuEdit::accept() {
	return;
}

void MenuEdit::cancel() {
	return;
}

void MenuEdit::setFocus(bool focus) {
	return;
}

bool MenuEdit::getFocus() const {
	return false;
}

void MenuEdit::display() {
	lcd->clear();
	lcd->setCursor(0, 0);
	lcd->print("%-11s%3dPa", (millis() - row1ErrCount < error_threshold) ? row1Text.c_str() : errText.c_str(), row1value);
	lcd->setCursor(0, 1);
	/* Ideally there should be error counting/checking for the second row to be consistent, but it would never be used in this project */
	lcd->print("%-11s%3dHz", row2Text.c_str(), row2value);
}

void MenuEdit::setRow1Values(int const currentValue, int const targetValue) {
	row1value = currentValue;
	if (currentValue == targetValue)
		row1ErrCount = millis();
}

void MenuEdit::setRow2Values(int const currentValue, int const targetValue) {
	row2value = currentValue;
	/* Not checking or doing anything with the targetValue */
}
