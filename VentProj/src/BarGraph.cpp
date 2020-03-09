/*
 * BarGraph.cpp
 *
 *  Created on: 1.2.2016
 *      Author: krl
 */

#include "BarGraph.h"

constexpr uint8_t BarGraph::horizontalMap[8][8];
constexpr uint8_t BarGraph::verticalMap[8][8];

BarGraph::BarGraph(LiquidCrystal& lcd_, int length_, int maxval, bool vertical_): lcd(lcd_), length(length_), maxval(maxval), vertical(vertical_) {
	int i;
	if (vertical) {
		length = 8;

		for(i = 0; i < 8; i++) {
			lcd.createChar(i, (uint8_t *) verticalMap[i]);
		}
		size = 1;
	}
	else {
		if(length < 5)
			length = 5;
		for(i = 0; i < 8; i++) {
			lcd.createChar(i, (uint8_t *) horizontalMap[i]);
		}
		size = length; // / 5 + (length % 5 > 0 ? 1 : 0);
	}
	lcd.setCursor(0,0); // move cursor back to visible area
}

BarGraph::~BarGraph() {
	// TODO Auto-generated destructor stub
}

void BarGraph::draw(int value) {
	if (value < 0) value = 0;
	if (value > maxval) value = maxval;

	if (vertical) {
		if(value == 0) lcd.write(32);
		else lcd.write(value - 1);
	}
	else {
		value = (size * 5)/(float(maxval)/value);
		int full = value / 5;
		int rest = value % 5;
		int count = size;
		while(count) {
			if(full > 0) {
				lcd.write(4);
				full--;
			}
			else if(rest > 0) {
				lcd.write(rest - 1);
				rest = 0;
			}
			else {
				lcd.write(32);
			}
			count--;
		}
	}
}
