/*
 * evolution.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: magnanim
 */

#include <stdio.h>
#include <vector>
#include <iostream>
#include "generation.h"

namespace mlnet {

void evolve(MLNetworkSharedPtr& mnet,
		long num_of_steps,
		long num_initial_actors,
		//std::vector<int> num_new_vertexes_per_step,
		const std::vector<double>& pr_internal_event,
		const std::vector<double>& pr_external_event,
		const matrix<double>& dependency,
		const std::vector<EvolutionModelSharedPtr>& evolution_model) {

	/* init of support data structures
	std::vector<std::set<entity_id> > free_identities(mnet.getNumNetworks());
	std::set<entity_id> ids = mnet.getGlobalIdentities();

	//std::cout << "Init identities" << std::endl;
	for (int n=0; n<mnet.getNumNetworks(); n++) {
		std::set<entity_id> to_copy(ids.begin(),ids.end());
		free_identities[n] = to_copy;
	}
	*/

	std::vector<double> pr_no_event;

	for (uint i=0; i<pr_internal_event.size(); i++) {
		pr_no_event.push_back(1-pr_internal_event.at(i)-pr_external_event.at(i));
	}

	// Creating num_initial_actors actors
	//std::cout << "INIT ACTORS" << std::endl;
	for (uint i=0; i<num_initial_actors; i++)
		mnet->add_actor("A"+std::to_string(i));

	// Initialization
	for (uint n=0; n<mnet->get_layers()->size(); n++) {
		//std::cout << "INIT LAYER " << n << std::endl;
		evolution_model[n]->init_step(mnet,mnet->get_layers()->get_at_index(n));
	}

	for (long i=0; i<num_of_steps; i++) {
		//std::cout << "step " << i << std::endl;
		for (uint n=0; n<mnet->get_layers()->size(); n++) {

			LayerSharedPtr target_layer = mnet->get_layers()->get_at_index(n);
			/* Add new vertexes ???
			//std::cout << "Add vertexes to " << n << ": ";
			for (int new_v=0; new_v<num_new_vertexes_per_step[n]; new_v++) {
				if (free_identities[n].empty())
					break;
				entity_id gid = rand.getElement(free_identities[n]);
				free_identities[n].erase(gid);
				vertex_id vertex = mnet.getNetwork(n).addVertex();
				mnet.mapIdentity(gid,vertex,n);
				//std::cout << ".";
			}
			//std::cout << std::endl;
			*/

			double dice = drand();

			if (dice < pr_no_event[n]) {
				//std::cout << "no event" << std::endl;
				//std::cout << " No event " << target_layer->to_string() << std::endl;
				// DO NOTHING;
			}

			//std::set<vertex_id> vertexes = mnet.getNetwork(n).getVertexes();
			//for (vertex_id vertex: vertexes) {
			else if (dice < pr_internal_event[n]+pr_no_event[n] || pr_external_event[n]==0) {
					/*
					//std::cout << "Iternal event for vertex " << vertex << std::endl;
					switch (strategy[n]) {
						case EVOLUTION_DEGREE:
							//std::cout << "Getting target vertex " << std::endl;
							vertex_id target = choice_degree(rand, mnet, n);
							if (target==-1) break;
							//std::cout << "Inserting edge to " << target << std::endl;
							if (!mnet.getNetwork(n).containsEdge(vertex,target))
								mnet.getNetwork(n).addEdge(vertex,target);
							break;
					}
					*/
				//std::cout << " Internal event " << target_layer->to_string() << std::endl;
				evolution_model[n]->evolution_step(mnet,target_layer);
			}
			else {
				// choose a layer from which to import an edge: first find the candidates:
				//std::set<network_id> candidates;
					uint layer_index = test(dependency[n]);
					LayerSharedPtr layer = mnet->get_layers()->get_at_index(layer_index);

					//std::cout << " External event " << target_layer->to_string() << " <- " << layer->to_string() << std::endl;

					// Choose an actor from that layer and replicate it to the target layer (if it does not already exist)
					NodeSharedPtr imported_node = mnet->get_nodes(layer)->get_at_random();
					NodeSharedPtr new_node = mnet->get_node(imported_node->actor,target_layer);
					if (!new_node)
						new_node = mnet->add_node(imported_node->actor,target_layer);

					// finally we insert the actor's neighbors into the target layer, if the neighbors are also present there
					for (NodeSharedPtr neighbor: *mnet->neighbors(imported_node,OUT)) {
						NodeSharedPtr local_neighbor = mnet->get_node(neighbor->actor,target_layer);
						if (local_neighbor) {
							if (!mnet->get_edge(new_node,local_neighbor)) {
								mnet->add_edge(new_node,local_neighbor);
							}
						}
					}
				}
		}
	}
}

/*
void evolve_edge_import(MultiplexNetwork &mnet,
		long num_of_steps,
		std::vector<double> pr_no_event,
		std::vector<double> pr_internal_event,
		std::vector<std::vector<double> > dependency,
		std::vector<EvolutionModel*> evolution_model) {

		Random rand;

		// check size
		for (int i=0; i<mnet.getNumNetworks(); i++) {
			evolution_model[i]->init_step(mnet, i);
		}

		for (long i=0; i<num_of_steps; i++) {
			//if (i%100==0) std::cout << "Step " << i << std::endl;
			for (int n=0; n<mnet.getNumNetworks(); n++) {
				//std::cout << "Network " << n << ": ";
				if (rand.test(pr_no_event[n])) {
					//std::cout << "no event" << std::endl;
					continue;
				}
				if (rand.test(pr_internal_event[n])) {
					//std::cout << "internal event" << std::endl;
					evolution_model[n]->evolution_step(mnet, n);
					continue;
				}
				else {
					// choose a network from which to import an edge: first find the candidates:
					std::set<network_id> candidates;
					for (int i=0; i<dependency[n].size(); i++) {
						if (rand.test(dependency[n][i]))
							candidates.insert(i);
					}
					// then pick uniformly at random one of the candidates
					network_id import = rand.getElement(candidates);
					//std::cout << "external event from " << import << std::endl;
					// finally we choose an edge uniformly at random from this network and we insert it into the target
					std::set<edge_id> edges = mnet.getNetwork(import).getEdges();
					edge_id edge_to_be_imported = rand.getElement(edges);
					entity_id id1 = mnet.getGlobalIdentity(edge_to_be_imported.v1, import);
					entity_id id2 = mnet.getGlobalIdentity(edge_to_be_imported.v2, import);
					// check if these identities are already present in the target network (if not, insert them)
					vertex_id vertex1, vertex2;
					if (!mnet.containsVertex(id1,n)) {
						if (mnet.getNetwork(n).isNamed())
							vertex1 = mnet.getNetwork(n).addVertex(mnet.getGlobalName(id1));
						else vertex1 = mnet.getNetwork(n).addVertex();
						mnet.mapIdentity(id1, vertex1, n);
					}
					else vertex1 = mnet.getVertexId(id1,n);
					if (!mnet.containsVertex(id2,n)) {
						if (mnet.getNetwork(n).isNamed())
							vertex2 = mnet.getNetwork(n).addVertex(mnet.getGlobalName(id2));
						else vertex2 = mnet.getNetwork(n).addVertex();
						mnet.mapIdentity(id2, vertex2, n);
					}
					else vertex2 = mnet.getVertexId(id2,n);
					if (!mnet.getNetwork(n).containsEdge(vertex1, vertex2))
						mnet.getNetwork(n).addEdge(vertex1, vertex2);
				}
			}
		}
}


void evolve_edge_copy(MultiplexNetwork &mnet,
		long num_of_steps,
		std::vector<double> pr_no_event,
		std::vector<double> pr_internal_event,
		std::vector<std::vector<double> > dependency,
		std::vector<EvolutionModel*> evolution_model) {

		Random rand;

		// check size
		for (int i=0; i<mnet.getNumNetworks(); i++) {
			evolution_model[i]->init_step(mnet, i);
		}

		for (long i=0; i<num_of_steps; i++) {
			//if (i%100==0) std::cout << "Step " << i << std::endl;
			for (int n=0; n<mnet.getNumNetworks(); n++) {
				//std::cout << "Network " << n << ": ";
				if (rand.test(pr_no_event[n])) {
					//std::cout << "no event" << std::endl;
					continue;
				}
				if (rand.test(pr_internal_event[n])) {
					//std::cout << "internal event " << n << std::endl;
					std::set<global_vertex_id> new_vertexes;
					std::set<global_edge_id> new_edges;
					evolution_model[n]->evolution_step(mnet, n, new_vertexes, new_edges);

					// The newly inserted vertexes and edges can be copied to other networks
					for (int i=0; i<dependency.size(); i++) {
						if (rand.test(dependency[i][n])) {
							//std::cout << dependency[i][n] << " copy to " << i << std::endl;
							/  * copy vertexes
							for (global_vertex_id gvid: new_vertexes) {
								global_identity gid = mnet.getGlobalIdentity(gvid.vid,gvid.network);
							if (!mnet.containsVertex(gid,i)) {
								vertex_id vertex;
								if (mnet.getNetwork(i).isNamed())
										vertex = mnet.getNetwork(i).addVertex(mnet.getGlobalName(gid));
								else
									vertex = mnet.getNetwork(i).addVertex();
								mnet.mapIdentity(gid, vertex, i);
							}
							}
							// copy edges
							for (global_edge_id geid: new_edges) {
								entity_id gid1 = mnet.getGlobalIdentity(geid.v1,geid.network);
								entity_id gid2 = mnet.getGlobalIdentity(geid.v2,geid.network);
								vertex_id lid1, lid2;
								if (!mnet.containsVertex(gid1,i)) { // not necessary
									if (mnet.getNetwork(i).isNamed())
										lid1 = mnet.getNetwork(i).addVertex(mnet.getGlobalName(gid1));
									else
										lid1 = mnet.getNetwork(i).addVertex();
									mnet.mapIdentity(gid1, lid1, i);
								}
								else lid1 = mnet.getVertexId(gid1,i);
								if (!mnet.containsVertex(gid2,i)) {
									if (mnet.getNetwork(i).isNamed())
										lid2 = mnet.getNetwork(i).addVertex(mnet.getGlobalName(gid2));
									else
										lid2 = mnet.getNetwork(i).addVertex();
									mnet.mapIdentity(gid2, lid2, i);
								}
								else lid2 = mnet.getVertexId(gid2,i);
								if (!mnet.getNetwork(i).containsEdge(lid1,lid2)) {
									mnet.getNetwork(i).addEdge(lid1,lid2);
									//std::cout << "E " << lid1 << " " << lid2 << std::endl;
								}
							}
						}
					}
				}
			}
		}
}

*/

}


