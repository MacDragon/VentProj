/*
 * BarGraph.cpp
 *
 *  Created on: 1.2.2016
 *      Author: krl
 */

#include "BarGraph.h"

constexpr uint8_t BarGraph::horizontalMap[8][8];
constexpr uint8_t BarGraph::verticalMap[8][8];

BarGraph::BarGraph(LiquidCrystal& lcd, int length, int maxval, bool vertical): lcd{ lcd }, length { length }, maxval{ maxval }, vertical{ vertical } {
	int i;
	if (vertical) {
		length = 8;

		for(i = 0; i < 8; i++)
			lcd.createChar(i, (uint8_t *) verticalMap[i]);
		size = 1;
	} else {
		if (length < 5)
			length = 5;

		for (i = 0; i < 8; i++)
			lcd.createChar(i, (uint8_t *) horizontalMap[i]);
		size = length;
	}
	lcd.setCursor(0, 0);
}

void BarGraph::draw(int value) {
	if (value < 0)
		value = 0;
	else if (value > maxval)
		value = maxval;

	if (vertical) {
		if (value == 0)
			lcd.write(32);
		else
			lcd.write(value - 1);
	} else {
		value = (size * 5) / (static_cast<float>(maxval) / value);
		int full = value / 5;
		int rest = value % 5;
		int count = size;
		while (count--) {
			if (full-- > 0)
				lcd.write(4);
			else if (rest > 0) {
				lcd.write(rest - 1);
				rest = 0;
			}
			else
				lcd.write(32);
		}
	}
}
