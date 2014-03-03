/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#import "test.h"
#include <iostream>
#include <string>

void testModularity() {

	// We need to read the network from a file: testIO() must have been passed
	log("TESTING measures");
	log("Reading the network...",false);
	// Creating an empty multiple network and initializing it
	MultipleNetwork mnet;
	//mnet_read_edgelist(mnet, "test/toy.mnet");
	mnet_read_edgelist(mnet, "/Users/matteomagnani/Dropbox/Research/13NetworkScience/code/fig6b.mnet");
	mnet.getNetwork("l1")->addVertex("v8");
	mnet.map("v8","v8","l1");
	mnet.getNetwork("l3")->addVertex("v8");
	mnet.map("v8","v8","l3");
	log("done!");

	log("Computing modularity...",false);
	std::map<network_id,std::map<vertex_id,long> > groups0, groups1, groups2, groups3;
	groups1[0] = std::map<vertex_id,long>();
	groups1[0][0]=0;
	groups1[0][1]=0;
	groups1[0][2]=0;
	groups1[0][3]=1;
	groups1[0][4]=1;
	groups1[0][6]=1;
	groups1[0][7]=1;
	groups1[1][0]=0;
	groups1[1][1]=0;
	groups1[1][2]=0;
	groups1[1][3]=1;
	groups1[1][4]=1;
	groups1[1][5]=1;
	groups1[1][6]=1;
	groups1[1][7]=1;
	groups1[2][0]=0;
	groups1[2][1]=0;
	groups1[2][2]=0;
	groups1[2][3]=1;
	groups1[2][4]=1;
	groups1[2][5]=1;
	groups1[2][6]=1;
	groups1[2][7]=1;

	groups2[0] = std::map<vertex_id,long>();
	groups2[0][0]=0;
	groups2[0][1]=0;
	groups2[0][2]=0;
	groups2[0][3]=1;
	groups2[0][4]=0;
	groups2[0][6]=1;
	groups2[0][7]=1;
	groups2[1][0]=0;
	groups2[1][1]=0;
	groups2[1][2]=0;
	groups2[1][3]=1;
	groups2[1][4]=1;
	groups1[1][5]=1;
	groups2[1][6]=1;
	groups2[1][7]=1;
	groups2[2][0]=0;
	groups2[2][1]=0;
	groups2[2][2]=0;
	groups2[2][3]=0;
	groups2[2][4]=1;
	groups2[2][5]=1;
	groups2[2][6]=1;
	groups2[2][7]=1;

	groups3[0] = std::map<vertex_id,long>();
	groups3[0][0]=0;
	groups3[0][1]=1;
	groups3[0][2]=0;
	groups3[0][3]=1;
	groups3[0][4]=0;
	groups3[0][6]=0;
	groups3[0][7]=1;
	groups3[1][0]=0;
	groups3[1][1]=1;
	groups3[1][2]=0;
	groups3[1][3]=1;
	groups3[1][4]=0;
	groups1[1][5]=1;
	groups3[1][6]=0;
	groups3[1][7]=1;
	groups3[2][0]=0;
	groups3[2][1]=1;
	groups3[2][2]=0;
	groups3[2][3]=1;
	groups3[2][4]=0;
	groups3[2][5]=1;
	groups3[2][6]=0;
	groups3[2][7]=1;

	log("GR1!");
	double mod = modularity(mnet,groups1,0);
	log(std::to_string(mod) + " ",false);
	log("done 0!");
	mod = modularity(mnet,groups1,.5);
	log(std::to_string(mod) + " ",false);
	log("done .5!");
	mod = modularity(mnet,groups1,1);
	log(std::to_string(mod) + " ",false);
	log("done 1!");
	log("GR2!");
	mod = modularity(mnet,groups2,0);
	log(std::to_string(mod) + " ",false);
	log("done 0!");
	mod = modularity(mnet,groups2,.5);
	log(std::to_string(mod) + " ",false);
	log("done .5!");
	mod = modularity(mnet,groups2,1);
	log(std::to_string(mod) + " ",false);
	log("done 1!");
	log("GR3!");
	mod = modularity(mnet,groups3,0);
	log(std::to_string(mod) + " ",false);
	log("done 0!");
	mod = modularity(mnet,groups3,.5);
	log(std::to_string(mod) + " ",false);
	log("done .5!");
	mod = modularity(mnet,groups3,1);
	log(std::to_string(mod) + " ",false);
	log("done 1!");

	log("TEST SUCCESSFULLY COMPLETED (modularity)");
}


