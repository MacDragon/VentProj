/*
 * ModeEdit.cpp
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#include "ModeEdit.h"

std::atomic<ModeEdit::Mode> ModeEdit::fanMode { Automatic };
constexpr char ModeEdit::kSubtitle[];
constexpr char ModeEdit::kSubUnit[];
constexpr unsigned int ModeEdit::kErrorThreshold;

ModeEdit::ModeEdit(LiquidCrystal& lcd, int const lowerLimit, int const upperLimit, Mode const mode)
:	lcd{ lcd }, bar{ BarGraph(lcd, 16, upperLimit - lowerLimit, false) }, lowerLimit{ lowerLimit }, upperLimit{ upperLimit },
	value{ lowerLimit }, edit{ lowerLimit }, focus{ false }, mode{ mode }, ErrTime{ millis() } {

	switch ( mode ) {
	case Manual :
		title = "Fan Speed";
		unit  = "%";
		break;

	case Automatic :
		title = "Pres Req";
		unit  = "Pa";
		break;
	}
}

void ModeEdit::increment() {
	if (edit < upperLimit)
		edit++;
}

void ModeEdit::decrement() {
	if (edit > lowerLimit)
		edit--;
}

void ModeEdit::change(int amount) {
	edit += amount;
	if (edit < lowerLimit)
		edit = lowerLimit;
	else if (edit > upperLimit)
		edit = upperLimit;
}

void ModeEdit::accept() {
	value = edit.load();
	ErrTime = millis();
}

inline void ModeEdit::cancel() {
	accept(); // new menu doesn't have an actual cancel.
}

void ModeEdit::setFocus(bool const focus) {
	this->focus = focus;
}

bool ModeEdit::getFocus() const {
	return focus;
}

void ModeEdit::display() {
	fanMode = mode;

	lcd.setCursor(0, 0);
	if (focus) { // item editor
		lcd.print("%-9s[%3d]%-2s", title.c_str(), edit.load(), unit.c_str());
		lcd.setCursor(0, 1);
		bar.draw(edit - lowerLimit);
	} else {
		if (millis() - ErrTime > kErrorThreshold)
			lcd.print("%-9s %3d %-2s", "Set Fail:", value.load(), unit.c_str());
		else
			lcd.print("%-9s %3d %-2s", title.c_str(), value.load(), unit.c_str());

		lcd.setCursor(0, 1);
		lcd.print("%-10s%3d %-2s", kSubtitle, observedValue, kSubUnit);
	}
}

int ModeEdit::getValue() const {
	return value;
}
void ModeEdit::setValue(int value) {
	if (value < lowerLimit)
		value = lowerLimit;
	else if (value > upperLimit)
		value = upperLimit;

	this->edit = value;
	this->value = value;
}

void ModeEdit::observe(Subject& subject) {
	subject.attach(this);
}

void ModeEdit::update(int value) {
	switch (mode) {
	case Manual:
		ErrTime = millis();
		observedValue = value;
		break;

	case Automatic:
		if (abs(this->value - value) <= 2)
			ErrTime = millis();
		observedValue = value;
		break;
	}
}
