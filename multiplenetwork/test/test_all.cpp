/*
 * run_all.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "test.h"
#include <stdlib.h>

void test_begin(const std::string& name) {
	std::cout << "*************************************************" << std::endl;
	std::cout << "*************************************************" << std::endl;
	std::cout << "** TESTING: " << name << std::endl;
	std::cout << "*************************************************" << std::endl;
}

void test_end(const std::string& name) {
	std::cout << "*************************************************" << std::endl;
	std::cout << "** COMPLETED (" << name << ")" << std::endl;
	std::cout << "*************************************************" << std::endl;
	std::cout << std::endl;
}

int main(int argn, char* argv[]) {

	try {
	test_datastructures();
	test_io();
	/*
	test_measures();
	testDistanceMeasures();
	testRandom();
	test_randomwalks();
	testEvolution();
	testModularity();
	testTransformations();
	testCommunity();
	*/
	}
	catch (std::exception& e) {
		std::cout << "[Error] UNIT TEST NOT PASSED: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


