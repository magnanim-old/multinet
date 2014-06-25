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

void evolve(MultiplexNetwork &mnet,
		long num_of_steps,
		double pr_no_event[],
		double pr_internal_event[],
		std::vector<std::vector<double> > dependency,
		std::vector<EvolutionModel*> evolution_model) {

	Random rand;

		// check size
		for (int i=0; i<mnet.getNumNetworks(); i++) {
			evolution_model[i]->init_step(mnet, i);
		}

		for (long i=0; i<num_of_steps; i++) {
			//std::cout << "Step " << i << std::endl;
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
					global_identity id1 = mnet.getGlobalIdentity(edge_to_be_imported.v1, import);
					global_identity id2 = mnet.getGlobalIdentity(edge_to_be_imported.v2, import);
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


