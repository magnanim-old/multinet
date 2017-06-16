/*
 * This program runs all the unit tests defined for this library.
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

int main() {

	test_begin("All unit tests");
	try {

	test_begin("UTILITY FUNCTIONS AND CLASSES");
	test_math();
	test_random();
	test_counter();
	test_csv();
	test_sortedrandomset();
	test_propertymatrix();
	test_end("UTILITY FUNCTIONS AND CLASSES");

	test_datastructures();
	test_io();
	test_measures();
	test_community();
	test_evolution();
	test_randomwalks();
	test_modularity();
	test_transformation();

	/*
	test_dynamics();
	*/
	}
	catch (std::exception& e) {
		std::cout << "[Error] UNIT TEST NOT PASSED: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	test_end("All unit tests");

	return EXIT_SUCCESS;
}


