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

void pareto_distance_all_paths(MultipleNetwork& mnet, vertex_id vertex,
		std::vector<std::set<Path> >& paths) {

	int num_networks = mnet.getNumNetworks();
	long num_vertexes = mnet.getNumVertexes();
	/* timestamp, used for efficiency reasons to avoid processing edges when no changes have occurred since the last iteration  */
	long ts = 0;
	/* last update timestamp */
	std::vector<std::map<vertex_id, std::map<vertex_id, long> > > last_updated;
	for (int network = 0; network < num_networks; network++) {
		std::map<vertex_id, std::map<vertex_id, long> > edge_timestamps;
		last_updated.push_back(edge_timestamps);
	}
	/* initialize distance array - for every target vertex there is still no found path leading to it...*/
	paths.resize(num_vertexes);
	for (int v = 0; v < num_vertexes; v++) {
		paths[v] = set<Path>();
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
		for (int network = 0; network < num_networks; network++) {
			//long num_edges = mnet.getNetwork(network)->getNumEdges();
			//cout << "processing network " << network << " (" << num_edges << " edges)\n";
			set<vertex_id> vertexes = mnet.getNetwork(network)->getVertexes();
			for (set<vertex_id>::iterator from_iterator = vertexes.begin();
					from_iterator != vertexes.end(); from_iterator++) {
				vertex_id from = *from_iterator;
				set<vertex_id> out_neighbors =
						mnet.getNetwork(network)->getOutNeighbors(from);
				for (set<vertex_id>::iterator to_iterator =
						out_neighbors.begin();
						to_iterator != out_neighbors.end(); to_iterator++) {
					vertex_id to = *to_iterator;
					//cout << std::to_string(from) << " " << std::to_string(to) << "\n";

					// initialize edge timestamp to -1 if first processed
					if (last_updated[network].count(from) == 0) {
						std::map<vertex_id, long> timestamps;
						last_updated[network][from] = timestamps;
					}
					if (last_updated[network][from].count(to) == 0) {
						last_updated[network][from][to] = -1;
					}
					long lastUpdate = last_updated[network][from][to];

					ts++;
					last_updated[network][from][to] = ts;

					//cout << "processing edge " << from << " => " << to << "\n";
					// global identifiers of the nodes connected by this edge
					long fromGlobalId = mnet.getGlobalVertexId(from, network);
					long toGlobalId = mnet.getGlobalVertexId(to, network);
					//cout << "processing edge " << from << " => " << to << "\n";
					//cout << "processing edge " << mnet.getGlobalVertexName(fromGlobalId) << " => " << mnet.getGlobalVertexName(toGlobalId) << "\n";

					// if no tmp shortest paths exist to edge.from, do nothing and continue
					if (paths[fromGlobalId].empty()) {
						continue;
					}
					//cout << "found distance to " << fromGlobalId <<  "\n";

					// otherwise, expand all tmp shortest paths to edge.from with [network][edge] and see if it generates a new shortest path to edge.to
					for (set<Path>::iterator path = paths[fromGlobalId].begin();
							path != paths[fromGlobalId].end(); ++path) {
						ts++;
						// no need to considered them if they where already there when we last checked this edge
						//debug("TS: " + p.timestamp + " " + lastUpdate);

						//cout << "processing path: " << *path << " "
						//		<< lastUpdate << "\n";
						if (path->getTimestamp() < lastUpdate) {
							//debug("  - " + p + " has not changed since " + e.lastChecked);
							continue;// TODO for efficiency: paths are sorted from most recently updated, so we do not need to examine the others
						}
						// otherwise, extend the path with this edge
						// TOADD: check it's not a cycle, for efficiency reasons (?)
						// Extend
						Path extended_path((*path), ts);
						extended_path.extend(to, network);

						// otherwise, compare it with the others
						bool should_be_inserted = true;
						set<Path> dominated;
						for (set<Path>::iterator previous_path =
								paths[toGlobalId].begin();
								previous_path != paths[toGlobalId].end();
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
							paths[toGlobalId].insert(extended_path);
							//cout << "insert\n";
							//cout << "add " << paths[toGlobalId].size() << "\n";
							//cout << "New path " << fromGlobalId << " => "
							//		<< toGlobalId << extended_path << "\n";
							changes = true;
						}

						// remove dominated paths
						set<Path> diff;
						set_difference(paths[toGlobalId].begin(),
								paths[toGlobalId].end(), dominated.begin(),
								dominated.end(), inserter(diff, diff.end()));
						paths[toGlobalId] = diff;

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

void pareto_betweenness(MultipleNetwork& mnet,	std::map<vertex_id, long>& vertex_betweenness) {
	for (vertex_id i = 0; i < mnet.getNumVertexes(); i++) {
		vector<set<Path> > paths;
		pareto_distance_all_paths(mnet, i, paths);
		for (unsigned long p = 0; p < paths.size(); p++) {
			//cout << "Node " << i << " to " << p << ": " << paths[p].size() << " paths\n";
			for (std::set<Path>::iterator path = paths[p].begin(); path != paths[p].end(); ++path) {
				//cout << *it << endl;
				//Path *path = &(*it);
				//cout << "b" << path << endl;
				for (long e = 1; e < path->length()-1; e++) {
					//long vertex2 = mnet.getGlobalVertexId(*to,net);
					if (vertex_betweenness.count(path->getVertex(e))==0)
						vertex_betweenness[path->getVertex(e)]=0;
					vertex_betweenness[path->getVertex(e)]++;
					//cout << "Increased " << mnet.getGlobalVertexName(vertex1) << " to " << betweenness[vertex1] << endl;
					//betweenness[vertex2]++;
				}
			}
		}
	}
	//cout << "mmm\n";
}

void pareto_edge_betweenness(MultipleNetwork& mnet, std::vector<std::map<vertex_id, std::map<vertex_id, long> > >& edge_betweenness) {
	edge_betweenness.resize(mnet.getNumNetworks());
	for (vertex_id i = 0; i < mnet.getNumVertexes(); i++) {
		vector<set<Path> > paths;
		pareto_distance_all_paths(mnet, i, paths);
		for (unsigned long p = 0; p < paths.size(); p++) {
			//cout << "Node " << i << " to " << p << ": " << paths[p].size() << " paths\n";
			for (std::set<Path>::iterator path = paths[p].begin(); path != paths[p].end(); ++path) {
				//cout << *it << endl;
				//Path *path = &(*it);
				//cout << "b" << path << endl;
				for (long e = 0; e < path->length()-1; e++) {
					vertex_id from = path->getVertex(e);
					vertex_id to = path->getVertex(e+1);
					network_id net = path->getNetwork(e);
					//long vertex2 = mnet.getGlobalVertexId(*to,net);
					if (edge_betweenness[net].count(from)==0) {
						std::map<vertex_id, long> to_edges;
						edge_betweenness[net][from]=to_edges;
					}
					if (edge_betweenness[net][from].count(to)==0) {
						edge_betweenness[net][from][to] = 0;
					}
					edge_betweenness[net][from][to]++;
					// We store only positive values of edge betweenness
					//cout << "Increased " << mnet.getGlobalVertexName(vertex1) << " to " << betweenness[vertex1] << endl;
					//betweenness[vertex2]++;
				}
			}
		}
	}
	//cout << "mmm\n";
}
