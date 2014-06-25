/*
 * degree.cpp
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "measures.h"
#include "transformation.h"
#include <iostream>


double network_jaccard_similarity(const MultiplexNetwork& mnet, const std::set<network_id>& active_networks) {
	/* verify if all networks are of the same kind
	bool directed = mnet.getNetwork(*active_networks.begin()).isDirected();
	std::set<network_id>::const_iterator it;
	for (it=active_networks.begin()+1; it!=active_networks.end(); ++it) {
		if (mnet.getNetwork(*it).isDirected() != directed)
			throw OperationNotSupportedException("cannot compare directed networks with undirected ones");
	}
	*/
	Network flat = flatten_weighted(mnet, active_networks);
	std::set<edge_id> edges = flat.getEdges();
	//std::cout << edges.size() << " " << flat.getNumEdges() << std::endl;
	long num_edges_on_all_networks = 0;
	for (edge_id e: edges) {
		if (flat.getEdgeWeight(e.v1,e.v2)==active_networks.size())
			num_edges_on_all_networks++;
	}
	//std::cout << num_edges_on_all_networks << " " << edges.size() << std::endl;
	return double(num_edges_on_all_networks)/edges.size();
}

double network_jaccard_similarity(const MultiplexNetwork& mnet, const std::set<std::string>& active_networks) {
	std::set<network_id> nets;
	for (std::string name: active_networks) {
		nets.insert(mnet.getNetworkId(name));
	}
	return network_jaccard_similarity(mnet,nets);
}

double network_jaccard_similarity(const MultiplexNetwork& mnet, network_id network1, network_id network2) {
	std::set<network_id> nets;
	nets.insert(network1);
	nets.insert(network2);
	return network_jaccard_similarity(mnet,nets);
}

double network_jaccard_similarity(const MultiplexNetwork& mnet, const std::string& network_name1, const std::string& network_name2) {
	std::set<std::string> nets;
	nets.insert(network_name1);
	nets.insert(network_name2);
	return network_jaccard_similarity(mnet,nets);
}

