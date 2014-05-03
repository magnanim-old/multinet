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
	MultilayerNetwork mnet_toy;
	mnet_read_edgelist(mnet_toy, "test/toy.mnet");
	mnet_toy.getNetwork("l1")->addVertex("U4");
	mnet_toy.map("U4","U4","l1");

	MultilayerNetwork mnet0;
	//mnet_read_edgelist(mnet, "test/toy.mnet");
	mnet_read_edgelist(mnet0, "/Users/matteomagnani/Dropbox/Research/13NetworkScience/code/fig6b.mnet");
	mnet0.getNetwork("l1")->addVertex("v8");
	mnet0.map("v8","v8","l1");
	mnet0.getNetwork("l3")->addVertex("v8");
	mnet0.map("v8","v8","l3");
	MultilayerNetwork mnet;
	//mnet_read_edgelist(mnet, "test/toy.mnet");
	mnet_read_edgelist(mnet, "/Users/matteomagnani/Dropbox/Research/13NetworkScience/code/fig6b.mnet");
	mnet.getNetwork("l1")->addVertex("v8");
	mnet.map("v8","v8","l1");
	mnet.getNetwork("l3")->addVertex("v8");
	mnet.map("v8","v8","l3");

	log("done!");

	log("Computing modularity...");
	std::map<network_id,std::map<vertex_id,long> > groups_toy, groups1, groups2, groups3, groups4;

	groups_toy[0][0]=0;
	groups_toy[0][1]=0;
	groups_toy[0][2]=0;
	groups_toy[0][3]=1;
	groups_toy[0][4]=1;
	groups_toy[1][1]=1;
	groups_toy[1][3]=1;
	groups_toy[1][4]=1;

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
	groups2[1][5]=1;
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
	groups3[1][5]=1;
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

	groups4[0][0]=0;
	groups4[0][1]=1;
	groups4[0][2]=0;
	groups4[0][3]=1;
	groups4[0][4]=0;
	groups4[0][6]=0;
	groups4[0][7]=1;
	groups4[1][0]=1;
	groups4[1][1]=0;
	groups4[1][2]=1;
	groups4[1][3]=0;
	groups4[1][4]=1;
	groups4[1][5]=0;
	groups4[1][6]=1;
	groups4[1][7]=0;
	groups4[2][0]=0;
	groups4[2][1]=1;
	groups4[2][2]=0;
	groups4[2][3]=1;
	groups4[2][4]=0;
	groups4[2][5]=1;
	groups4[2][6]=0;
	groups4[2][7]=1;

	double mod = modularity(mnet_toy,groups_toy,1);
	log(std::to_string(mod) + " ",false);
	log("done 1 - toy!");


	log("GR1!");
	mod = modularity(mnet0,groups1,0);
	log(std::to_string(mod) + " ",false);
	log("done 0!");
	mod = modularity(mnet0,groups1,.5);
	log(std::to_string(mod) + " ",false);
	log("done .5!");
	mod = modularity(mnet0,groups1,1);
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
	log("GR4!");
	mod = modularity(mnet,groups4,0);
	log(std::to_string(mod) + " ",false);
	log("done 0!");
	mod = modularity(mnet,groups4,.5);
	log(std::to_string(mod) + " ",false);
	log("done .5!");
	mod = modularity(mnet,groups4,1);
	log(std::to_string(mod) + " ",false);
	log("done 1!");

	log("TEST SUCCESSFULLY COMPLETED (modularity)");
}


