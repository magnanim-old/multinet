/*
 * test.h
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#ifndef MULTIPLENETWORK_TEST_H_
#define MULTIPLENETWORK_TEST_H_

//#include "multiplenetwork.h"
#include <exception>
#include <string>

/* Exceptions */
class FailedUnitTestException: public std::exception {
public:
	FailedUnitTestException(std::string message);
	~FailedUnitTestException() throw ();
	virtual const char* what() const throw();
private:
	std::string message;
};

/* Unit test functions */
//void testNetwork();
void testMLNetwork();
void testIO();
void testLocalMeasures();
void testRandom();
void testRandomWalks();
/*void testDistanceMeasures();
void testModularity();
void testEvolution();
void testTransformations();*/


#endif /* MULTIPLENETWORK_TEST_H_ */
