/*
 * test.h
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#ifndef MULTIPLENETWORK_TEST_H_
#define MULTIPLENETWORK_TEST_H_

#include <exception>
#include <string>
#include <iostream>

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
void test_datastructures();
void test_io();
void test_measures();
void testDistanceMeasures();
void testRandom();
void test_randomwalks();
void test_evolution();
void testModularity();
/*
void testTransformations();
void testCommunity();
*/

void test_begin(const std::string& name);

void test_end(const std::string& name);


#endif /* MULTIPLENETWORK_TEST_H_ */
