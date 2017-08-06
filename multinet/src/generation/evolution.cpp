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

MLNetworkSharedPtr evolve(
		long num_of_steps,
		size_t num_actors,
		const std::vector<double>& pr_internal_event,
		const std::vector<double>& pr_external_event,
		const matrix<double>& dependency,
		const std::vector<EvolutionModelSharedPtr>& evolution_model) {

    MLNetworkSharedPtr mnet = MLNetwork::create("synth");
    
    
	std::vector<double> pr_no_event;
	for (size_t i=0; i<pr_internal_event.size(); i++) {
		pr_no_event.push_back(1-pr_internal_event.at(i)-pr_external_event.at(i));
	}
    
    // Creating num_layers layers
    size_t num_layers = pr_internal_event.size();
    for (size_t i=0; i<num_layers; i++)
        mnet->add_layer("L"+std::to_string(i),UNDIRECTED);
    
	// Creating num_actors actors
	for (size_t i=0; i<num_actors; i++)
		mnet->add_actor("@"+std::to_string(i));
    
	// Initialization
    std::vector<ActorListSharedPtr> available_actors;
    for (size_t n=0; n<mnet->get_layers()->size(); n++) {
        available_actors.push_back(std::make_shared<actor_list>());
        for (ActorSharedPtr actor: *mnet->get_actors()) {
            available_actors[n]->insert(actor);
        }
		evolution_model[n]->init_step(mnet,mnet->get_layers()->get_at_index(n),available_actors[n]);
    }
    
    // Evolution
	for (long i=0; i<num_of_steps; i++) {
		for (size_t n=0; n<mnet->get_layers()->size(); n++) {

			LayerSharedPtr target_layer = mnet->get_layers()->get_at_index(n);

			double dice = drand();

            if (dice < pr_no_event[n]) {
				// DO NOTHING;
			}
            else if (dice < pr_internal_event[n]+pr_no_event[n] || pr_external_event[n]==0) {
                // INTERNAL EVOLUTION
                evolution_model[n]->internal_evolution_step(mnet,target_layer,available_actors[n]);
            }
            else {
                // EXTERNAL EVOLUTION
				// choose a layer from which to import edges.
				uint layer_index = test(dependency[n]);
                LayerSharedPtr external_layer = mnet->get_layers()->get_at_index(layer_index);
                
                evolution_model[n]->external_evolution_step(mnet,target_layer,available_actors[n],external_layer);
            }
		}
	}
    return mnet;
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


