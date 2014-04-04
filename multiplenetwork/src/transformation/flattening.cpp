/*
 * flattening.cpp
 *
 * Created on: Mar 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#import "transformation.h"
#import <set>


// Only for named networks
void flatten(MultipleNetwork& mnet, MNET_FLATTENING_ALGORITHM algorithm,
		Network& net) {

	std::set<global_vertex_id> vertexes;
	mnet.getVertexes(vertexes);
	std::set<global_vertex_id>::const_iterator v_it;
	for (v_it = vertexes.begin(); v_it != vertexes.end(); ++v_it) {
		net.addVertex(mnet.getVertexName(*v_it));
	}

	std::set<global_edge_id> edges;
	mnet.getEdges(edges);
	std::set<global_edge_id>::const_iterator e_it;
	for (e_it = edges.begin(); e_it != edges.end(); ++e_it) {
		if (! net.containsEdge(mnet.getVertexName((*e_it).v1),mnet.getVertexName((*e_it).v2)))
			net.addEdge(mnet.getVertexName((*e_it).v1),mnet.getVertexName((*e_it).v2));
	}
}

