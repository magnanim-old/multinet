/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#import "test.h"
#import "datastructures.h"
#import "exceptions.h"
#import "utils.h"
#import "io.h"

void testIO() {
	log("TESTING IO of multiple networks");
	log("Reading multiple network from file...",false);
	MultilayerNetwork mnet;
	mnet_read_edgelist(mnet, "test/toy.mnet");
	log("done!");
	print(mnet);

	// TODO Check network properties
	// log("TEST SUCCESSFULLY COMPLETED (Network class)");
}


