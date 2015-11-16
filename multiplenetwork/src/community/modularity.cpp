/*
 * modularity.cpp
 *
 * Created on: Feb 27, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "community.h"
#include "measures.h"
#include <unordered_map>
#include <iostream>

namespace mlnet {

double modularity(MLNetworkSharedPtr mnet, const hash<NodeSharedPtr,long>& membership, double c) {
	// partition the nodes by group
	hash<long, std::set<NodeSharedPtr> > groups;
	for (auto pair: membership) {
		groups[pair.second].insert(pair.first);
	}
	// start computing the modularity
	double res = 0;
	double mu = 0;
	hash<LayerSharedPtr,long> m_s;
	for (LayerSharedPtr s: mnet->get_layers()) {
		double m = mnet->get_edges(s,s).size();
		if (!mnet->is_directed(s,s))
			m *= 2;
		mu += m;
		// FIX TO THE ORIGINAL EQUATION WHEN THERE ARE NO EDGES
		if (m == 0)
			m = 1; // no effect on the formula
		m_s[s] = m;
		mu += m;
	}

		for (auto pair: groups) {
			for (NodeSharedPtr i: pair.second) {
				for (NodeSharedPtr j: pair.second) {
				if (i==j) continue; // not in the original definition - we do this assuming to deal with simple graphs
				//std::cout << i->to_string() << " " << groups.count(i) << std::endl;
				//std::cout << j->to_string() << " " << groups.count(j) << std::endl;

				if (i->layer==j->layer) {
					//std::cout << "Same group!" << std::endl;
					//if (mnet.getNetwork(net)->containsEdge(*v_i,*v_j))
					//	std::cout << "Edge" << std::endl;
					long k_i = mnet->neighbors(i,OUT).size();
					long k_j = mnet->neighbors(j,IN).size();
					int a_ij = mnet->get_edge(i,j)? 1.0 : 0.0;
					res += a_ij - k_i * k_j / (2 * m_s.at(i->layer));
					//std::cout << global_v_i << " " << global_v_j << " " << (a_ij - k_i * k_j / (2 * m_net)) << std::endl;
					//std::cout << "->" << res << std::endl;
				}
				if (i->actor==j->actor) {
					res += c;
				}
			}
		}
		//std::cout << "->" << m_net << std::endl;
	}
	//std::cout << "same" << std::endl;

	for (ActorSharedPtr actor: mnet->get_actors()) {
		int num_nodes = mnet->get_nodes(actor).size();
		mu+=c*num_nodes*(num_nodes-1);
	}
	//std::cout << "->" << mod << " " << (res-mod) << "-" << mu2 << std::endl;

	return 1 / mu * res;
}

}
