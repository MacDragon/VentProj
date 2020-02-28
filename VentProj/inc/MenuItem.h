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
	enum menuEvent {
		up,
		down,
		ok,
		back,
		show,
		change
	};

	MenuItem(PropertyEdit* const property);
	virtual ~MenuItem();
	virtual bool event(menuEvent e, int amount = 1);
private:
	PropertyEdit* const pe;
};

#endif /* MENUITEM_H_ */
