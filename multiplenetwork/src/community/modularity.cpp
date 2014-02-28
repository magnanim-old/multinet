/*
 * modularity.cpp
 *
 * Created on: Feb 27, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "community.h"
#include "measures.h"

double modularity(MultipleNetwork& mnet,
		std::map<network_id,std::map<vertex_id,int> > groups) {
	double res = 0;
	for (std::map<network_id,std::map<vertex_id,int> >::iterator network_iterator=groups.begin();
			network_iterator!=groups.end(); network_iterator++) {
		network_id net = (*network_iterator).first;
		double m_net = mnet.getNetwork(net)->getNumEdges();
		// FIX TO THE ORIGINAL EQUATION WHEN THERE ARE NO EDGES
		if (m_net == 0)
			continue;
		std::set<vertex_id> vertexes;
		mnet.getVertexes(vertexes);
		for (std::set<vertex_id>::iterator v_i = vertexes.begin();
				v_i != vertexes.end(); v_i++) {
			for (std::set<vertex_id>::iterator v_j = vertexes.begin();
					v_j != vertexes.end(); v_j++) {
				if (mnet.getNetwork(net)->containsVertex(
						mnet.getLocalVertexId(*v_i, net))
						&& mnet.getNetwork(net)->containsVertex(
								mnet.getLocalVertexId(*v_j, net))
						&& groups[net][mnet.getLocalVertexId(*v_i, net)]
								== groups[net][mnet.getLocalVertexId(*v_j, net)]) {
					long k_i = degree(mnet, *v_i, net);
					long k_j = degree(mnet, *v_j, net);
					int a_ij =
							mnet.getNetwork(net)->containsEdge(
									mnet.getLocalVertexId(*v_i, net),
									mnet.getLocalVertexId(*v_j, net)) ? 1 : 0;
					res += a_ij - k_i * k_j / (2 * m_net);
					//cout << "Nodes " <<  i <<  " and " << j << " -> " << ( 1 - k_i*k_j/(2*m_net)) << "\n";
				}
			}
		}
	}

	/*
	 for (mnet_network_t s=0; s<mnet->getNumNetworks(); s++) {
	 for (mnet_network_t r=0; r<mnet->getNumNetworks(); r++) {
	 for (mnet_vertex_t v=0; v<mnet->getNumVertexes(); v++) {
	 if (mnet->containsLocalVertex(v,s) &&
	 mnet->containsLocalVertex(v,r) &&
	 groups[s][mnet->getLocalVertexId(v,s)] == groups[r][mnet->getLocalVertexId(v,r)]) {
	 res += 1; // or omega
	 //cout << "Node " << v << " (" << groups[s][mnet->getLocalVertexId(v,s)] << ") on same custer on "
	 //	<< s <<  " and " << r << "\n";

	 }

	 }
	 }
	 }
	 */
	double mu = 0;
	for (network_id net = 0; net < mnet.getNumNetworks(); net++) {
		mu += mnet.getNetwork(net)->getNumEdges();
	}
	/*
	 for (mnet_vertex_t v=0; v<mnet->getNumVertexes(); v++) {
	 int networks_with_v = 0;
	 for (mnet_network_t net=0; net<mnet->getNumNetworks(); net++) {
	 if (mnet->containsLocalVertex(v,net)) networks_with_v++;
	 }
	 mu += networks_with_v*(networks_with_v-1)/2;
	 }*/
	return 1 / (2 * mu) * res;
}
