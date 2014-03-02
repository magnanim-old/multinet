/*
 * modularity.cpp
 *
 * Created on: Feb 27, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "community.h"
#include "measures.h"
#include <map>
#include <iostream>

double modularity(MultipleNetwork& mnet,
		 std::map<network_id,std::map<vertex_id,long> >& groups, double c) {
	double res = 0;
	double mu = 0;
	std::set<network_id> networks;
	mnet.getNetworks(networks);
	std::set<network_id>::iterator network_iterator;
	for (network_iterator=networks.begin(); network_iterator!=networks.end(); network_iterator++) {
		network_id net = *network_iterator;
		double m_net = mnet.getNetwork(net)->getNumEdges();
		mu +=m_net;
		// FIX TO THE ORIGINAL EQUATION WHEN THERE ARE NO EDGES
		if (m_net == 0)
			continue;
		std::set<vertex_id> vertexes;
		mnet.getNetwork(net)->getVertexes(vertexes);
		for (std::set<vertex_id>::iterator v_i = vertexes.begin();
				v_i != vertexes.end(); v_i++) {
			for (std::set<vertex_id>::iterator v_j = vertexes.begin();
					v_j != vertexes.end(); v_j++) {
				// if (*v_i==*v_j) continue; SEEMS REASONABLE, BUT NOT IN DEFINITION...
				vertex_id global_v_i = mnet.getGlobalVertexId(*v_i, net);
				vertex_id global_v_j = mnet.getGlobalVertexId(*v_j, net);
				//std::cout << global_v_i << " " << global_v_j << std::endl;

				if (groups[net][global_v_i] == groups[net][global_v_j]) {
					//std::cout << "Same group!" << std::endl;
					//if (mnet.getNetwork(net)->containsEdge(*v_i,*v_j))
					//	std::cout << "Edge" << std::endl;
					long k_i = degree(mnet, global_v_i, net);
					long k_j = degree(mnet, global_v_j, net);
					int a_ij =
							mnet.getNetwork(net)->containsEdge(
									*v_i,
									*v_j) ? 1.0 : 0.0;
					res += a_ij - k_i * k_j / (2 * m_net);
					//std::cout << global_v_i << " " << global_v_j << " " << (a_ij - k_i * k_j / (2 * m_net)) << std::endl;
					//std::cout << "->" << res << std::endl;
				}
			}
		}
		//std::cout << "->" << res << std::endl;
	}

	double mod = res;

	std::set<network_id>::iterator network_iterator1, network_iterator2;
	std::set<vertex_id>::iterator vertex_iterator;
	std::set<vertex_id> vertexes;
	mnet.getVertexes(vertexes);
	for (network_iterator1=networks.begin(); network_iterator1!=networks.end(); ++network_iterator1) {
		network_id net1 = *network_iterator1;
		for (network_iterator2=networks.begin(); network_iterator2!=networks.end(); ++network_iterator2) {
			network_id net2 = *network_iterator2;
			if (net1==net2) continue;
			for (vertex_iterator=vertexes.begin(); vertex_iterator!=vertexes.end(); ++vertex_iterator) {
				 vertex_id v = *vertex_iterator;
				 mu++;
				 if (mnet.containsVertex(v,net1) &&
					mnet.containsVertex(v,net2) &&
					groups[net1][v] == groups[net2][v]) {
					 	 res += c; // or omega
			 	}
			}
		}
	}

	std::cout << "->" << mod << " " << (res-mod) << std::endl;

	return 1 / (2 * mu) * res;
}
