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

/*
 void pareto_distance(MultipleNetwork* mnet,
 igraph_integer_t vertex,
 vector<set<MultiDistance> > *distances) {}
*/

std::map<entity_id,std::set<Distance> > pareto_distance(const MLNetwork& mnet, entity_id identity)  {
	std::map<entity_id,std::set<Distance> > distances;
	std::set<entity_id> identities = mnet.getGlobalIdentities();
	/* timestamps, used for efficiency reasons to avoid processing edges when no changes have occurred since the last iteration  */
	long ts = 0;
	std::map<global_edge_id, long> last_updated;
	/* initialize distance array - for every target vertex there is still no found path leading to it...*/
	for (entity_id id: identities) {
		distances[id] = set<Distance>();
	} // ...except for the source node, reachable from itself via an empty path
	Distance empty(mnet.getNumNetworks(), ts);
	distances[identity].insert(empty);

	bool changes; // keep updating the paths until when no changes occur during one full scan of the edges
	do {
		changes = false;
		//set<global_edge_id> edges = mnet.getEdges();
		for (network_id network: mnet.getNetworks()) {
			for (vertex_id from: mnet.getNetwork(network).getVertexes()) {
				for (vertex_id to: mnet.getNetwork(network).getOutNeighbors(from)) {
					//vertex_id from = mnet.getGlobalIdentity(local_from, network);
					//vertex_id to = mnet.getGlobalIdentity(local_to, network);

					// initialize edge timestamp to -1 if first processed
					global_edge_id e(from, to, network, true);
					//cout << "N: " << network << " " << e.network << endl;
					long lastUpdate;
					if (last_updated.count(e)==0) lastUpdate = -1;
					else lastUpdate = last_updated[e];
					ts++;
					last_updated[e] = ts;
					//cout << ts << " --> " << from << " (" << mnet.getGlobalName(mnet.getGlobalIdentity(e.v1,e.network)) <<  ") to " << to << " (" << mnet.getGlobalName(mnet.getGlobalIdentity(e.v2,e.network)) << ") on " << e.network << endl;


					entity_id global_identity_1 = mnet.getGlobalIdentity(e.v1,e.network);
					entity_id global_identity_2 = mnet.getGlobalIdentity(e.v2,e.network);

					// if no tmp shortest paths exist to e.v1, do nothing and continue
					if (distances[global_identity_1].empty()) {
						//cout << "No paths to " << mnet.getGlobalName(global_identity_1) << endl;
						continue;
					}
					// otherwise, expand each temporary distance to e.v1 and see if it generates a new shortest path to e.v2
					for (Distance dist: distances[global_identity_1]) {
						ts++;
						if (dist.getTimestamp() < lastUpdate) {
							//cout << "Already processed: " << lastUpdate << ": "<< dist << endl;
							// distance already processed
							continue;// TODO for efficiency: paths are sorted e.v1 most recently updated, so we do not need to examine the others
						}
						// otherwise, extend the distance to reach e.v2
						// TOADD: check it's not a cycle, for efficiency reasons (?)
						// Extend
						Distance extended_distance(dist, ts);
						extended_distance.extend(e.network);
						//cout << "producing candidate: " << extended_distance << endl;

						// compare the new distance with the other temporary distances to e.v2
						bool should_be_inserted = true;
						set<Distance> dominated; // here we store the distances that will be removed if dominated by the new one
						for (Distance previous: distances[global_identity_2]) {
							// check dominance, i.e., if this is a shorter distance
							//cout << "comparison " << extended_distance << " vs. " << previous << ": ";

							int dominance = check_dominance(extended_distance,previous);
							switch (dominance) {
							case P_DOMINATED: // stop here
								should_be_inserted = false;
								//cout << "dominated" << endl;

								break;
							case P_EQUAL:
								// this means that the number of steps in each network is the same.
								// Only one of them will be kept
								should_be_inserted = false;
								//cout << "equal" << endl;
								// go on with the others
								break;
							case P_INCOMPARABLE: // incomparable
								//cout << "inc." << endl;
								// go on with the others
								break;
							case P_DOMINATES: // dominates -> insert it in the list of paths to be removed
								dominated.insert(previous);
								//cout << " dominated - remove " << mnet.getGlobalName(global_identity_2) << " - " << previous << endl;
								//debug("   - REMOV " + currentPath);
								break;
							}
						}

						if (should_be_inserted) {
							distances[global_identity_2].insert(extended_distance);
							//cout << "insert " << mnet.getGlobalName(global_identity_2) << " - " << extended_distance << endl;
							//cout << "add " << paths[toGlobalId].size() << "\n";
							//cout << "New path " << fromGlobalId << " => "
							//		<< toGlobalId << extended_path << "\n";
							changes = true;
						}

						// remove dominated paths
						set<Distance> diff;
						set_difference(distances[global_identity_2].begin(),
								distances[global_identity_2].end(), dominated.begin(),
								dominated.end(), inserter(diff, diff.end()));
						distances[global_identity_2] = diff;

					}

				}
				//System.out.println();
			}
		}
		//if (!changes) break;
	} while (changes);
	return distances;
	//cout << "here?\n";
	//paths->assign(paths.begin(), paths.end());
	//cout << "here?\n";
}

std::map<entity_id,std::set<Path> > pareto_distance_all_paths(const MLNetwork& mnet, entity_id vertex) {
	std::map<entity_id,std::set<Path> > paths;
	std::set<entity_id> vertexes = mnet.getGlobalIdentities();
	/* timestamp, used for efficiency reasons to avoid processing edges when no changes have occurred since the last iteration  */
	long ts = 0;
	/* last update timestamp */
	std::map<global_edge_id, long> last_updated;
	/*
	for (int network = 0; network < num_networks; network++) {
		std::map<vertex_id, std::map<vertex_id, long> > edge_timestamps;
		last_updated.push_back(edge_timestamps);
	}*/
	/* initialize distance array - for every target vertex there is still no found path leading to it...*/
	//paths.resize(num_vertexes);
	for (std::set<entity_id>::iterator v = vertexes.begin(); v != vertexes.end(); v++) {
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

		set<global_edge_id> edges = mnet.getEdges();
		//for (set<global_edge_id>::iterator edge_iterator=edges.begin(); edge_iterator!=edges.end(); ++edge_iterator) {
		//	global_edge_id e=(*edge_iterator);
		//	if (!e.directed) {
		//
		//	}
		std::set<network_id> nets = mnet.getNetworks();
		std::set<network_id>::iterator network_iterator;
		for (network_iterator=nets.begin(); network_iterator!=nets.end(); ++network_iterator) {
			network_id network = *network_iterator;

			//long num_edges = mnet.getNetwork(network)->getNumEdges();
			//cout << "processing network " << network << " (" << num_edges << " edges)\n";
			set<vertex_id> vertexes = mnet.getNetwork(network).getVertexes();
			for (set<vertex_id>::iterator from_iterator = vertexes.begin();
					from_iterator != vertexes.end(); from_iterator++) {
				vertex_id local_from = *from_iterator;
				set<vertex_id> out_neighbors = mnet.getNetwork(network).getOutNeighbors(local_from);
				for (set<vertex_id>::iterator to_iterator =
						out_neighbors.begin();
						to_iterator != out_neighbors.end(); to_iterator++) {
					vertex_id local_to = *to_iterator;
					//cout << std::to_string(from) << " " << std::to_string(to) << "\n";

					vertex_id from = mnet.getGlobalIdentity(local_from, network);
					vertex_id to = mnet.getGlobalIdentity(local_to, network);
					global_edge_id e(from, to, network, true);
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
						//if (path->length()>=bound)
						//	continue;

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
							case P_DOMINATED: // stop here
								should_be_inserted = false;
								break;
							case P_EQUAL:
								// this means that the number of steps in each network is the same.
								// Both will be kept, except if it is the same path
								if (extended_path.same(*previous_path))
									should_be_inserted = false;
								// go on with the others
								break;
							case P_INCOMPARABLE: // incomparable
								// go on with the others
								break;
							case P_DOMINATES: // dominates -> insert it in the list of paths to be removed
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
	return paths;
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
			return P_INCOMPARABLE;
	}
	if (canDominate && !canBeDominated)
		return P_DOMINATES;
	if (canBeDominated && !canDominate)
		return P_DOMINATED;
	//if (canDominate && canBeDominated)
	return P_EQUAL;
}

int check_dominance(const Distance& p1, const Distance& p2) {
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
			return P_INCOMPARABLE;
	}
	if (canDominate && !canBeDominated)
		return P_DOMINATES;
	if (canBeDominated && !canDominate)
		return P_DOMINATED;
	//if (canDominate && canBeDominated)
	return P_EQUAL;
}
