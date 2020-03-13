/*
 * MenuItem.cpp
 *
 *  Created on: 1.2.2016
 *      Author: krl
 */

#include "MenuItem.h"

bool MenuItem::event(MenuEvent e, int amount) {
	bool handled = true;

	switch(e) {
	case ok:
		// do nothing, ok is now used to move between items not start edit.
		if (pe.getFocus()) {
			pe.setFocus(false);
			pe.accept();
		}
		else
			handled = false;
		break;

	case back:
		if (pe.getFocus()) {
			pe.setFocus(false);
			pe.cancel();
		}
		else
			handled = false;
		break;

	case show:
		break;

	case up:
		if (pe.getFocus())
			pe.increment();
		else
			pe.setFocus(true);
		break;

	case down:
		if (pe.getFocus())
			pe.decrement();
		else
			pe.setFocus(true);
		break;

	case change:
		if (pe.getFocus())
			pe.change(amount);
		else
			pe.setFocus(true);
	break;
	}

	if (handled)
		pe.display();

	return handled;
}

