/*
 * test.h
 *
 * Unit testing functions.
 *
 */

#ifndef MLNET_TEST_H_
#define MLNET_TEST_H_

#include <exception>
#include <string>
#include <cmath>
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

/* exception, thrown if a test fails */
class FailedUnitTestException: public std::exception {
public:
	FailedUnitTestException(std::string message);
	~FailedUnitTestException() throw ();
	virtual const char* what() const throw();
private:
	std::string message;
};

void test_community_single_layer();
void test_flattening();
void test_pmm();
void test_lart();
void test_glouvain();

void test_math();
void test_random();
void test_counter();
void test_csv();
void test_sortedrandommap();
void test_sortedrandomset();
void test_propertymatrix();

void test_datastructures();
void test_io();
void test_measures();
void test_transformation();
void test_randomwalks();
void test_evolution();
void test_modularity();
void test_community();
void test_dynamics();

#endif /* MLNET_TEST_H_ */
