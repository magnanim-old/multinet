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

/*void pareto_distance(MultipleNetwork* mnet,
 igraph_integer_t vertex,
 vector<set<MultiDistance> > *distances) {}*/

void pareto_distance_all_paths(MultilayerNetwork& mnet, vertex_id vertex,
		std::map<vertex_id,std::set<Path> >& paths) {
	pareto_distance_all_paths(mnet, vertex, paths, 1000000);
}


void pareto_distance_all_paths(MultilayerNetwork& mnet, vertex_id vertex,
		std::map<vertex_id,std::set<Path> >& paths, int bound) {

	std::set<vertex_id> vertexes;
	mnet.getVertexes(vertexes);
	/* timestamp, used for efficiency reasons to avoid processing edges when no changes have occurred since the last iteration  */
	long ts = 0;
	/* last update timestamp */
	std::map<intralayer_edge_id, long> last_updated;
	/*
	for (int network = 0; network < num_networks; network++) {
		std::map<vertex_id, std::map<vertex_id, long> > edge_timestamps;
		last_updated.push_back(edge_timestamps);
	}*/
	/* initialize distance array - for every target vertex there is still no found path leading to it...*/
	//paths.resize(num_vertexes);
	for (std::set<vertex_id>::iterator v = vertexes.begin(); v != vertexes.end(); v++) {
		paths[(*v)] = set<Path>();
	} // ...except for the source node, reachable from itself via an empty path
	Path empty(mnet, ts);
	empty.start(vertex);
	paths[vertex].insert(empty);

	bool changes; // keep updating the paths until when no changes occur during one full scan of the edges
	do {
		//DEBUG
		//for (int i=0; i<num_vertexes; i++) {
		//	cout << paths[i].size() << " ";
		//}
		//cout << "\n";
		//log("Round " + round);
		changes = false;
		//int counter = 0;

		set<intralayer_edge_id> edges;
		mnet.getEdges(edges);
		//for (set<global_edge_id>::iterator edge_iterator=edges.begin(); edge_iterator!=edges.end(); ++edge_iterator) {
		//	global_edge_id e=(*edge_iterator);
		//	if (!e.directed) {
		//
		//	}
		std::set<network_id> nets;
		mnet.getNetworks(nets);
		std::set<network_id>::iterator network_iterator;
		for (network_iterator=nets.begin(); network_iterator!=nets.end(); ++network_iterator) {
			network_id network = *network_iterator;

			//long num_edges = mnet.getNetwork(network)->getNumEdges();
			//cout << "processing network " << network << " (" << num_edges << " edges)\n";
			set<vertex_id> vertexes;
			mnet.getNetwork(network)->getVertexes(vertexes);
			for (set<vertex_id>::iterator from_iterator = vertexes.begin();
					from_iterator != vertexes.end(); from_iterator++) {
				vertex_id local_from = *from_iterator;
				set<vertex_id> out_neighbors;
				mnet.getNetwork(network)->getOutNeighbors(local_from, out_neighbors);
				for (set<vertex_id>::iterator to_iterator =
						out_neighbors.begin();
						to_iterator != out_neighbors.end(); to_iterator++) {
					vertex_id local_to = *to_iterator;
					//cout << std::to_string(from) << " " << std::to_string(to) << "\n";

					vertex_id from = mnet.getGlobalVertexId(local_from, network);
					vertex_id to = mnet.getGlobalVertexId(local_to, network);
					intralayer_edge_id e(from, to, false, network);
					// initialize edge timestamp to -1 if first processed


					long lastUpdate;
					if (last_updated.count(e)==0) lastUpdate = -1;
					else lastUpdate = last_updated[e];

					ts++;
					last_updated[e] = ts;

					//cout << "processing edge " << e.v1 << " => " << to << "\n";
					// global identifiers of the nodes connected by this edge
					//cout << "processing edge " << e.v1 << " => " << to << "\n";
					//cout << "processing edge " << mnet.getGlobalVertexName(e.v1GlobalId) << " => " << mnet.getGlobalVertexName(toGlobalId) << "\n";

					// if no tmp shortest paths exist to edge.e.v1, do nothing and continue
					if (paths[e.v1].empty()) {
						continue;
					}
					//cout << "found distance to " << e.v1GlobalId <<  "\n";

					// otherwise, expand all tmp shortest paths to edge.e.v1 with [network][edge] and see if it generates a new shortest path to edge.to
					for (set<Path>::iterator path = paths[e.v1].begin();
							path != paths[e.v1].end(); ++path) {
						ts++;
						// no need to considered them if they where already there when we last checked this edge
						//debug("TS: " + p.timestamp + " " + lastUpdate);

						// if
						if (path->length()>=bound)
							continue;

						//cout << "processing path: " << *path << " "
						//		<< lastUpdate << "\n";
						if (path->getTimestamp() < lastUpdate) {
							//debug("  - " + p + " has not changed since " + e.lastChecked);
							continue;// TODO for efficiency: paths are sorted e.v1 most recently updated, so we do not need to examine the others
						}
						// otherwise, extend the path with this edge
						// TOADD: check it's not a cycle, for efficiency reasons (?)
						// Extend
						Path extended_path((*path), ts);
						extended_path.extend(e.v2, e.network);

						// otherwise, compare it with the others
						bool should_be_inserted = true;
						set<Path> dominated;
						for (set<Path>::iterator previous_path =
								paths[e.v2].begin();
								previous_path != paths[e.v2].end();
								++previous_path) {
							//cout << "previous path: " << *path << "\n";

							// check domination, i.e., if this is a tmp shortest path
							int dominance = check_dominance(extended_path,
									*previous_path);
							//cout << "DOM: " << dominance << "\n";
							switch (dominance) {
							case PATH_DOMINATED: // stop here
								should_be_inserted = false;
								break;
							case PATH_EQUAL:
								// this means that the number of steps in each network is the same.
								// Both will be kept, except if it is the same path
								if (extended_path.same(*previous_path))
									should_be_inserted = false;
								// go on with the others
								break;
							case PATH_INCOMPARABLE: // incomparable
								// go on with the others
								break;
							case PATH_DOMINATES: // dominates -> insert it in the list of paths to be removed
								dominated.insert(*previous_path);
								//debug("   - REMOV " + currentPath);
								break;
							}
						}

						if (should_be_inserted) {
							paths[e.v2].insert(extended_path);
							//cout << "insert\n";
							//cout << "add " << paths[toGlobalId].size() << "\n";
							//cout << "New path " << fromGlobalId << " => "
							//		<< toGlobalId << extended_path << "\n";
							changes = true;
						}

						// remove dominated paths
						set<Path> diff;
						set_difference(paths[e.v2].begin(),
								paths[e.v2].end(), dominated.begin(),
								dominated.end(), inserter(diff, diff.end()));
						paths[e.v2] = diff;

					}

				}
				//System.out.println();
			}
		}
		//if (!changes) break;
	} while (changes);
	//cout << "here?\n";
	//paths->assign(paths.begin(), paths.end());
	//cout << "here?\n";
}

int check_dominance(const Path& p1, const Path& p2) {
	bool canBeDominated = true;
	bool canDominate = true;
	if (p1.getNumNetworks() != p2.getNumNetworks()) {
		// throw Exception
	}

	for (int i = 0; i < p1.getNumNetworks(); i++) {
		if (p1.getNumEdgesOnNetwork(i) > p2.getNumEdgesOnNetwork(i)) {
			canDominate = false;
		} else if (p1.getNumEdgesOnNetwork(i) < p2.getNumEdgesOnNetwork(i)) {
			canBeDominated = false;
		}

		if (!canBeDominated && !canDominate)
			return PATH_INCOMPARABLE;
	}
	if (canDominate && !canBeDominated)
		return PATH_DOMINATES;
	if (canBeDominated && !canDominate)
		return PATH_DOMINATED;
	//if (canDominate && canBeDominated)
	return PATH_EQUAL;
}
