/*
 * SimpleMenu.cpp
 *
 *  Created on: 3.2.2016
 *      Author: krl
 */

#include "SimpleMenu.h"

SimpleMenu::SimpleMenu() : position{ 0 } {}

SimpleMenu::~SimpleMenu() {
	// TODO Auto-generated destructor stub
}

/* Takes a pointer to a Property Edit object and wraps it
 * in a unique pointer to a Menu Item object. Kind of
 * pointless since it depends on the PropertyEdit object
 * having the same lifetime as the SimpleMenu object (it does). */
void SimpleMenu::addItem(PropertyEdit* pe) {
	items.emplace_back(new MenuItem(pe));
}

void SimpleMenu::event(MenuItem::menuEvent e) {
	const std::lock_guard<Imutex> lock(mutex);

	if (items.size() <= 0)
		return;

	if (!items[position]->event(e)) {
		if (e == MenuItem::up && ++position >= static_cast<int>(items.size()))
			position = 0;
		else if (e == MenuItem::down && --position < 0)
			position = items.size() - 1;
		else if (e == MenuItem::back)
			position = 0;

		items[position]->event(MenuItem::show);
	}
}
