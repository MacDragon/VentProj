/*
 * SimpleMenu.h
 *
 *  Created on: 3.2.2016
 *      Author: krl
 */

#ifndef SIMPLEMENU_H_
#define SIMPLEMENU_H_
#include <vector>
#include <memory>
#include "PropertyEdit.h"
#include "MenuItem.h"

class SimpleMenu {
public:
	SimpleMenu();
	virtual ~SimpleMenu();
	void addItem(PropertyEdit* pe);
	void event(MenuItem::menuEvent e);

private:
	std::vector<std::unique_ptr<MenuItem>> items;
	int position;
};

#endif /* SIMPLEMENU_H_ */
