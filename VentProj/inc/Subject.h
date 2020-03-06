/*
 * Subject.h
 *
 *  Created on: 6 Mar 2020
 *      Author: Joshua
 */

#ifndef SUBJECT_H_
#define SUBJECT_H_

#include "PropertyEdit.h"

class Subject {
public:
	virtual ~Subject() {};

	virtual void attach(PropertyEdit* observer) = 0;
	virtual void notify() = 0;
};

#endif /* SUBJECT_H_ */
