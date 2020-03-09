/*
 * PID.h
 *
 *  Created on: 19 Feb 2020
 *      Author: Joshua
 */

#ifndef PID_H_
#define PID_H_

/* This is templated because in most cases you would probably want
 * the PID controller to process floating point numbers. However,
 * in this project we will only ever be passing integers to the PID controller. */

template <typename T>
class PID {
public:
	constexpr PID(double const Kp, double const Ki, double const Kd) :
	Kp{ Kp }, Ki{ Ki }, Kd{ Kd }, sum_of_errors{ 0 }, last_error{ 0 } {}
	virtual ~PID() {}

	double calculate(T const target, T const current) {
		T error = target - current;
		sum_of_errors += error;

		double P = Kp * error;
		double I = Ki * sum_of_errors;
		double D = Kd * (error - last_error);

		last_error = error;

		return P + I + D;
	}

private:
	double const Kp, Ki, Kd;
	T sum_of_errors;
	T last_error;
};

#endif /* PID_H_ */
