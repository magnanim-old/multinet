/*
 * test.h
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#ifndef MLNET_TEST_H_
#define MLNET_TEST_H_

#include <exception>
#include <string>
#include <iostream>
#include "print.h"

/**
 * Prints a text to the standard output indicating that the test is going to be performed.
 * @param name the name of the test
 */
void test_begin(const std::string& name);

/**
 * Prints a text to the standard output indicating that the test has been performed.
 * @param name the name of the test
 */
void test_end(const std::string& name);

/* Unit test functions for different components of the library */
void test_utils();
void test_datastructures();
void test_io();
void test_measures();
void test_transformations();
void test_random();
void test_randomwalks();
void test_evolution();
void test_modularity();
void test_community();
void test_dynamics();

/* exception, thrown if a test fails */
class FailedUnitTestException: public std::exception {
public:
	FailedUnitTestException(std::string message);
	~FailedUnitTestException() throw ();
	virtual const char* what() const throw();
private:
	std::string message;
};

#endif /* MLNET_TEST_H_ */
