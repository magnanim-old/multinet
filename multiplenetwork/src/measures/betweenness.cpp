/*
 * distances.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: magnanim
 */

#include "measures.h"
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;

void pareto_betweenness(MultipleNetwork& mnet,	std::map<vertex_id, long>& vertex_betweenness) {
	for (vertex_id i = 0; i < mnet.getNumVertexes(); i++) {
		// initialize the result structure
		vertex_betweenness[i] = 0;
		map<vertex_id,set<Path> > paths;
		pareto_distance_all_paths(mnet, i, paths);
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
	//cout << "mmm\n";
}

void pareto_edge_betweenness(MultipleNetwork& mnet, std::map<global_edge_id, long>& edge_betweenness) {
	for (vertex_id i = 0; i < mnet.getNumVertexes(); i++) {
		map<vertex_id,set<Path> > paths;
		pareto_distance_all_paths(mnet, i, paths);
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
					global_edge_id step(from,to,mnet.getNetwork(e)->isDirected(),net);
					//long vertex2 = mnet.getGlobalVertexId(*to,net);
					if (edge_betweenness.count(step)==0) {
						edge_betweenness[step] = 0;
					}
					edge_betweenness[step]++;
					// We store only positive values of edge betweenness
				}
			}
		}
	}
	//cout << "mmm\n";
}
