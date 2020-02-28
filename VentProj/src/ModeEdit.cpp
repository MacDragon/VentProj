/*
 * ModeEdit.cpp
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#include <ModeEdit.h>
#include <cstdio>

std::atomic<ModeEdit::Mode> ModeEdit::fanMode{ Automatic };

ModeEdit::ModeEdit(LiquidCrystal* lcd, int const lowerLimit, int const upperLimit, Mode const mode) :
lcd{ lcd }, bar{ BarGraph(lcd, 14, upperLimit-lowerLimit, false) }, lowerLimit{ lowerLimit }, upperLimit{ upperLimit }, value{ lowerLimit }, edit{ lowerLimit },
focus{ false }, mode{ mode }, ErrCount{ millis() }  {

	switch ( mode ) {
	case Manual :
		title = "Fan Speed";
		subTitle = "Fan Speed";
		editUnit = "%";
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
		break;
	case Startup :
		title = "";
		subTitle = "";
		editUnit = "";
		dispTitle = "";
		dispUnit = "";
		break;
	}
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

void ModeEdit::change(int amount) {
	edit+=amount;
	if (edit < lowerLimit)
		edit = lowerLimit;
	if (edit > upperLimit)
		edit = upperLimit;
}

void ModeEdit::accept() {
	value = edit;
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
//	lcd->clear();
	lcd->setCursor(0, 0);
	if(focus) { // item editor
		lcd->print("%-9s[%3d]%s", subTitle.c_str(), edit, editUnit.c_str());
		// maybe draw bargraph here too
		lcd->setCursor(0, 1);
		lcd->print("[              ]");
		lcd->setCursor(1, 1);
		bar.draw(edit-lowerLimit);
	} else {
		lcd->print("%-9s %3d %s", title.c_str(), value, editUnit.c_str());
		lcd->setCursor(0, 1);
		// second display value
	//	lcd->print("%-9s%3d Pa", (millis() - ErrCount > error_threshold) ? row2Text.c_str() : errText.c_str(), row2value);
		lcd->print("%-10s%3d %s", dispTitle.c_str(), value2, dispUnit.c_str());
	}

}

int ModeEdit::getValue() const {
	return value;
}
void ModeEdit::setValue(int const value) {
	this->value = this->edit = value;
	if ( mode == Automatic )
		if (abs(value2-value) > 1)
			ErrCount = millis();
}

void ModeEdit::setDispValue2(const int value) {
	this->value2 = value;
}


