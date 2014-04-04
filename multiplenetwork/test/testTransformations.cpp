/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#import "test.h"
#import "multiplenetwork.h"

void testTransformations() {
	log("TESTING transformations...",false);
	log("Reading multiple network from file...",false);
	MultipleNetwork mnet, output;
	mnet_read_edgelist(mnet, "test/toy.mnet");
	log("done!");

	Network net(true,false,false);

	flatten(mnet,MNET_OR_FLATTENING,net);

	output.addNetwork("flattened",net);
	std::set<global_vertex_id> vertexes;
	mnet.getVertexes(vertexes);
	std::set<global_vertex_id>::const_iterator v_it;
	for (v_it = vertexes.begin(); v_it != vertexes.end(); ++v_it) {
		output.addVertex(mnet.getVertexName(*v_it));
		output.map(mnet.getVertexName(*v_it),mnet.getVertexName(*v_it),"flattened");
	}


	log("Flattened network:");
	print(output);
}


