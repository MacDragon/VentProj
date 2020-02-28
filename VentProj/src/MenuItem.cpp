/*
 * MenuItem.cpp
 *
 *  Created on: 1.2.2016
 *      Author: krl
 */

#include "MenuItem.h"

MenuItem::MenuItem(PropertyEdit* const property) : pe{ property } {}

MenuItem::~MenuItem() {}

bool MenuItem::event(menuEvent e, int amount) {
	bool handled = true;

	switch(e) {
	case ok:
		// do nothing, ok is not used to move between items not start edit.
		handled = false;
/*		if(pe->getFocus()) {
			pe->setFocus(false);
			pe->accept();
		}
		else
			pe->setFocus(true); */
		break;

	case back:
		if(pe->getFocus()) {
			pe->setFocus(false);
			pe->cancel();
		}
		else
			handled = false;
		break;

	case show:
		break;

	case up:
		if(pe->getFocus())
			pe->increment();
		else
			pe->setFocus(true);
//			handled = false;
		break;

	case down:
		if(pe->getFocus())
			pe->decrement();
		else
				pe->setFocus(true);
//			handled = false;
		break;

	case change:
		if(pe->getFocus())
			pe->change(amount);
		else
			pe->setFocus(true);
//			handled = false;
	break;
	}


	if(handled)
		pe->display();

	return handled;
}

