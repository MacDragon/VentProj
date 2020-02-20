/*
 * PID.h
 *
 *  Created on: 19 Feb 2020
 *      Author: Joshua
 */

#ifndef PID_H_
#define PID_H_

class PID {
public:
	constexpr PID(double const Kp, double const Ki, double const Kd) :
	Kp{ Kp }, Ki{ Ki }, Kd{ Kd }, sum_of_errors{ 0 }, min{ 0 }, max{ 20000 }, last_error{ 0 } {}
	virtual ~PID();

	double calculate(unsigned int const target, unsigned int const current) {
		int error = target - current;
		sum_of_errors += error;

		double P = Kp * error;
		double I = Ki * sum_of_errors;
		double D = Kd * (error - last_error);

		last_error = error;

		return P + I + D;
	}
private:
	double const Kp, Ki, Kd;
	double sum_of_errors;
	unsigned int const  min, max;
	int last_error;
};

#endif /* PID_H_ */
