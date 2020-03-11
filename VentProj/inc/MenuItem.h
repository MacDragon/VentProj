/*
 * MenuItem.h
 *
 *  Created on: 1.2.2016
 *      Author: krl
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
