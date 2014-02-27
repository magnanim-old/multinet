/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#import "test.h"
#import "multiplenetwork.h"
#include <iostream>

void testMeasures() {

	// We need to read the network from a file: testIO() must have successfully passed
	log("TESTING measures");
	log("Creating the network...",false);
	MultipleNetwork mnet;
	mnet_read_edgelist(mnet, "data/toy.mnet");

	log("TESTING Pareto distance between all pairs of vertexes");
	std::vector<std::set<Path> > paths;
	pareto_distance_all_paths(mnet, mnet.getVertexId("U0"), paths);
	for (int v=0; v<paths.size(); v++) {
		log("Distance to vertex " + mnet.getVertexName(v));
		for (std::set<Path>::iterator it=paths[v].begin(); it!=paths[v].end(); it++) {
			std::cout << *it;
		}
	}

	std::map<vertex_id, long> vertex_betweenness;
	pareto_betweenness(mnet, vertex_betweenness);

	log("Pareto betweenness:");
	for (std::map<vertex_id, long>::iterator btw=vertex_betweenness.begin(); btw!=vertex_betweenness.end(); btw++) {
		std::cout << mnet.getVertexName((*btw).first) << ": " << (*btw).second << std::endl;
	}
	std::vector<std::map<vertex_id, std::map<vertex_id, long> > > edge_betweenness;
	pareto_edge_betweenness(mnet, edge_betweenness);

	log("Pareto edge betweenness:");
	for (network_id net=0; net<mnet.getNumNetworks(); net++) {
	for (std::map<vertex_id,std::map<vertex_id, long> >::iterator from=edge_betweenness[net].begin(); from!=edge_betweenness[net].end(); from++) {
		for (std::map<vertex_id, long>::iterator to=vertex_betweenness.begin(); to!=vertex_betweenness.end(); to++) {
			std::cout << mnet.getVertexName((*from).first) << " -" << mnet.getNetworkName(net) << "-> " << mnet.getVertexName((*to).first) << ": " << (*to).second << std::endl;
		}
	}
	}

}


