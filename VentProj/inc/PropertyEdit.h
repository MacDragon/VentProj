/*
 * PropertyEdit.h
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#ifndef PROPERTYEDIT_H_
#define PROPERTYEDIT_H_

/* Frankstein's monster interface that also follows the observer pattern */
class PropertyEdit {
public:
	virtual ~PropertyEdit() {};
	virtual void increment() = 0;
	virtual void decrement() = 0;
	virtual void change(int amount) = 0;
	virtual void accept() = 0;
	virtual void cancel() = 0;
	virtual void setFocus(bool focus) = 0;
	virtual bool getFocus() const = 0;
	virtual void display() = 0;

	/* Observer pattern */
	virtual void update(int value) = 0;
};

#endif /* PROPERTYEDIT_H_ */
