/*
 * SimpleMenu.cpp
 *
 *  Created on: 3.2.2016
 *      Author: krl
 */

#include "SimpleMenu.h"

SimpleMenu::SimpleMenu() : position{ 0 } { /* Empty */ }

void SimpleMenu::addItem(PropertyEdit& pe) {
	items.emplace_back(new MenuItem(pe));
}

void SimpleMenu::event(MenuItem::MenuEvent e, int amount) {
	const std::lock_guard<Imutex> lock(mutex);

	if (items.size() <= 0)
		return;

	if (!items[position]->event(e, amount)) {
		if (e == MenuItem::ok && ++position >= static_cast<int>(items.size())) // Use ok button to proceed through menu
			position = 0;

		items[position]->event(MenuItem::show);
	}
}
