/*
 * distances.cpp
 *
 */

#include "measures.h"
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <ctime>

using namespace std;

std::map<entity_id, long> pareto_betweenness(const MultiplexNetwork& mnet) {
	std::map<entity_id, long> vertex_betweenness;
	std::clock_t start;
	    double duration;

	    start = std::clock();

	for (entity_id i = 0; i < mnet.getNumVertexes(); i++) {
		duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
		//std::cout << "Vertex " << i << "," << duration << endl;
		// initialize the result structure
		vertex_betweenness[i] = 0;
		map<entity_id,set<Path> > paths = pareto_distance_all_paths(mnet, i);
		for (unsigned long p = 0; p < paths.size(); p++) {
			//cout << "Node " << i << " to " << p << ": " << paths[p].size() << " paths\n";
			for (std::set<Path>::iterator path = paths[p].begin(); path != paths[p].end(); ++path) {
				//cout << *it << endl;
				//Path *path = &(*it);
				//cout << "b" << path << endl;
				for (long e = 1; e < path->length(); e++) {
					//long vertex2 = mnet.getGlobalVertexId(*to,net);
					vertex_betweenness[path->getVertex(e)]++;
					//cout << "Increased " << mnet.getGlobalVertexName(vertex1) << " to " << betweenness[vertex1] << endl;
					//betweenness[vertex2]++;
				}
			}
		}
	}
	return vertex_betweenness;
	//cout << "mmm\n";
}

std::map<global_edge_id, long> pareto_edge_betweenness(const MultiplexNetwork& mnet) {
	std::map<global_edge_id, long> edge_betweenness;
	for (vertex_id i = 0; i < mnet.getNumVertexes(); i++) {
		map<vertex_id,set<Path> > paths = pareto_distance_all_paths(mnet, i);
		for (unsigned long p = 0; p < paths.size(); p++) {
			//cout << "Node " << i << " to " << p << ": " << paths[p].size() << " paths\n";
			for (std::set<Path>::iterator path = paths[p].begin(); path != paths[p].end(); ++path) {
				//cout << *it << endl;
				//Path *path = &(*it);
				//cout << *path << endl;
				for (long e = 0; e < path->length(); e++) {
					vertex_id from = path->getVertex(e);
					vertex_id to = path->getVertex(e+1);
					network_id net = path->getNetwork(e);
					global_edge_id edge(from,to,mnet.getNetwork(e).isDirected(),net);
					//long vertex2 = mnet.getGlobalVertexId(*to,net);
					if (edge_betweenness.count(edge)==0) {
						edge_betweenness[edge] = 0;
					}
					edge_betweenness[edge]++;
					// We store only positive values of edge betweenness
				}
			}
		}
	}
	return edge_betweenness;
	//cout << "mmm\n";
}
