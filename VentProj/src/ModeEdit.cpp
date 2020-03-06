/*
 * ModeEdit.cpp
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#include <ModeEdit.h>
#include <cstdio>

std::atomic<ModeEdit::Mode> ModeEdit::fanMode{ Automatic };

ModeEdit::ModeEdit(Subject& subject, LiquidCrystal& lcd, int const lowerLimit, int const upperLimit, Mode const mode)
:	subject{ subject }, lcd{ lcd }, bar{ BarGraph(lcd, 16, upperLimit-lowerLimit, false) }, lowerLimit{ lowerLimit }, upperLimit{ upperLimit }, value{ lowerLimit }, edit{ lowerLimit },
	focus{ false }, mode{ mode }, ErrTime{ millis() } {

	subject.attach(this);

	switch ( mode ) {
	case Manual :
		title = "Fan Speed";
		subTitle = "Fan Speed";
		editUnit = "% ";
		dispTitle = "Cur Pres";
		dispUnit = "Pa";
		break;

	case Automatic :
		title = "Pres Req";
		subTitle = "Pres Req";
		editUnit = "Pa";
		dispTitle = "Cur Pres";
		dispUnit = "Pa";
		break;
	}
}

ModeEdit::~ModeEdit() { /* Empty */ }

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
	value = edit;
	ErrTime = millis();
}

void ModeEdit::cancel() {
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
		lcd.print("%-9s[%3d]%s", subTitle.c_str(), edit, editUnit.c_str());
		lcd.setCursor(0, 1);
		bar.draw(edit-lowerLimit);
	} else {
		if (millis() - ErrTime > error_threshold)
			lcd.print("%-9s %3d %s", "Set Fail:", value, editUnit.c_str());
		else
			lcd.print("%-9s %3d %s", title.c_str(), value, editUnit.c_str());

		lcd.setCursor(0, 1);
		lcd.print("%-10s%3d %s", dispTitle.c_str(), observedValue, dispUnit.c_str());
	}

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

int ModeEdit::getValue() const {
	return value;
}
void ModeEdit::setValue(int value) {
	if (value < lowerLimit)
		value = lowerLimit;

	this->value = this->edit = value;
}


