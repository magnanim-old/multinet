/*
 * run_all.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "test.h"
#include <stdlib.h>

int main(int argn, char* argv[]) {

	testNetwork();
	testMultilayerNetwork();
	testMultiplex();
	testIO();
	testTransformations();
	testMeasures();
	//testModularity(); //Still testing this, but seems to work (it does for single networks)
	testRandom();
	testEvolution();

	return EXIT_SUCCESS;
}


