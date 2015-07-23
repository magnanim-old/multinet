/*
 * run_all.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "test.h"
#include <stdlib.h>
#include <iostream>

int main(int argn, char* argv[]) {

	try {
	testMLNetwork();
	testIO();
	testLocalMeasures();
	/*
	testTransformations();
	testDistanceMeasures();
	//testModularity(); //Still testing this, but seems to work so far (it does for single networks)
	testRandom();
	testEvolution();*/
	}
	catch (std::exception& e) {
		std::cout << "[Error] UNIT TEST NOT PASSED: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


