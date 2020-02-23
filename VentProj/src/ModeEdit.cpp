/*
 * ModeEdit.cpp
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#include <ModeEdit.h>
#include <cstdio>

std::atomic<ModeEdit::Mode> ModeEdit::fanMode{ Startup };

ModeEdit::ModeEdit(LiquidCrystal* lcd, std::string const& title, std::string const& subTitle, int const lowerLimit, int const upperLimit, Mode const mode) :
lcd{ lcd }, title{ title }, subTitle{ subTitle },
lowerLimit{ lowerLimit }, upperLimit{ upperLimit }, value{ lowerLimit }, edit{ lowerLimit },
focus{ false }, mode{ mode } {
	/* Empty */
}

ModeEdit::~ModeEdit() {
	/* Empty */
}

void ModeEdit::increment() {
	if (edit < upperLimit)
		edit++;
}

void ModeEdit::decrement() {
	if (edit > lowerLimit)
		edit--;
}

void ModeEdit::accept() {
	value = edit;
	fanMode = mode;
}

void ModeEdit::cancel() {
	edit = value;
}

void ModeEdit::setFocus(bool const focus) {
	this->focus = focus;
}

bool ModeEdit::getFocus() const {
	return focus;
}

void ModeEdit::display() {
	lcd->clear();
	lcd->setCursor(0, 0);
	if(focus) {
		lcd->print("%s", subTitle.c_str());
		lcd->setCursor(0, 1);
		lcd->print("     [%4d]     ", edit);
	} else {
		lcd->print(title);
		lcd->setCursor(0, 1);
		lcd->print("       %4s     ", mode == fanMode ? "[ON]" : "OFF");
	}
}

int ModeEdit::getValue() const {
	return value;
}
void ModeEdit::setValue(int const value) {
	this->value = this->edit = value;
}


