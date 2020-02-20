/*
 * SimpleMenu.h
 *
 *  Created on: 3.2.2016
 *      Author: krl
 */

#ifndef SIMPLEMENU_H_
#define SIMPLEMENU_H_
#include <vector>
#include "Imutex.h"
#include "MenuItem.h"

class SimpleMenu {
public:
	SimpleMenu();
	virtual ~SimpleMenu();
	void addItem(MenuItem *item);
	void event(const MenuItem::menuEvent& e);
	int  getPosition() const { return position; }
private:
	std::vector<MenuItem*> items;
	int position;
	Imutex mutex;
};

#endif /* SIMPLEMENU_H_ */
