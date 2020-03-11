/*
 * SimpleMenu.h
 *
 *  Created on: 3.2.2016
 *      Author: krl
 */

#ifndef SIMPLEMENU_H_
#define SIMPLEMENU_H_

#include "PropertyEdit.h"
#include "MenuItem.h"
#include "Imutex.h"
#include <memory>
#include <vector>

class SimpleMenu {
public:
	SimpleMenu();
	void addItem(PropertyEdit& pe);
	void event(MenuItem::MenuEvent e, int amount = 1);

private:
	std::vector<std::unique_ptr<MenuItem>> items;
	int position;
	Imutex mutex;
};

#endif /* SIMPLEMENU_H_ */
