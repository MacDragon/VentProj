/*
 * MenuItem.h
 *
 *  Originally Created on: 2.2.2016
 *      Author: krl
 *  Modified: March 2020
 *  	Authors: Joshua Waugh & Visa Harvey
 */

#ifndef MENUITEM_H_
#define MENUITEM_H_

#include "PropertyEdit.h"

class MenuItem {
public:
	enum MenuEvent { up, down, ok, back, show, change };

	constexpr MenuItem(PropertyEdit& property) : pe{ property } { /* Empty */ }
	bool event(MenuEvent e, int amount = 1);

private:
	PropertyEdit& pe;
};

#endif /* MENUITEM_H_ */
