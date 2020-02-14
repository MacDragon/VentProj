/*
 * ITM.h
 *
 *  Created on: 17 Jan 2020
 *      Author: visa
 */

#ifndef ITMOUT_H_
#define ITMOUT_H_

#include <string>

#define BUFLEN 100

class ITMOut {
public:
	ITMOut();
	void print( const char *);
	void print( const std::string &output );
	void print( const int );

	virtual ~ITMOut();
private:
	char buffer[BUFLEN] = {0};
};

#endif /* ITMOUT_H_ */
