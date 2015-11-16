/*
 * evolution.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: magnanim
 */

#include <stdio.h>
#include <vector>
#include <iostream>
#include "evolution.h"

/*
vertex_id choice_degree(Random rand, MultiplexNetwork& mnet, network_id net) {
	std::set<edge_id> all_edges = mnet.getNetwork(net).getEdges();
	//std::cout << "Found " << all_edges.size() << " edges." << std::endl;
	if (all_edges.size()==0) {
		std::set<vertex_id> all_vertexes = mnet.getNetwork(net).getVertexes();
		if (all_vertexes.size()==0) return -1;
		vertex_id vertex = rand.getElement(all_vertexes);
		return vertex;
	}
	edge_id edge = rand.getElement(all_edges);
	//std::cout << "Got edge " << edge << std::endl;
	return rand.test(.5)?edge.v1:edge.v2;
}
*/
