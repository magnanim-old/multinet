/*
 * flattening.cpp
 *
 * Created on: Mar 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#import "transformation.h"
#import <set>
#import <iostream>

// Only for named networks
Network flatten_weighted(const MultiplexNetwork& mnet, const std::set<network_id>& active_networks) {
	// verify if there are directed networks: if yes, the resulting flattened network is also directed
	bool directed = false;
	for (network_id network: active_networks) {
		if (mnet.getNetwork(network).isDirected()) {
			directed = true;
			continue;
		}
	}
	Network net(true,directed,true);

	std::set<std::string> vertexNames = mnet.getGlobalNames();
	for (std::string name: vertexNames) {
		net.addVertex(name);
	}

	//std::cout << "BEGIN flattening" << std::endl;


	std::set<global_edge_id> edges = mnet.getEdges();
	//std::cout << edges.size() << std::endl;
	for (global_edge_id e: edges) {
		// not very efficient implementation when only a few networks are flattened...
		if (active_networks.count(e.network)==0) continue;

		std::string vertex_name1 = mnet.getGlobalName(mnet.getGlobalIdentity(e.v1,e.network));
		std::string vertex_name2 = mnet.getGlobalName(mnet.getGlobalIdentity(e.v2,e.network));
		if (!net.containsEdge(vertex_name1,vertex_name2)) {
			net.addEdge(vertex_name1,vertex_name2,1);
			//std::cout << vertex_name1 << " -new- " << vertex_name2 << " " << 1 << std::endl;
		}
		else {
			net.setEdgeWeight(vertex_name1,vertex_name2,net.getEdgeWeight(vertex_name1,vertex_name2)+1);
			//std::cout << vertex_name1 << " -ex- " << vertex_name2 << " " << net.getEdgeWeight(vertex_name1,vertex_name2) << std::endl;
		}
		// if the resulting network is directed, undirected edges must be inserted as two directed ones
		if (directed && !e.directed) {
			if (!net.containsEdge(vertex_name2,vertex_name1)) {
				net.addEdge(vertex_name2,vertex_name1,1);
				//std::cout << vertex_name2 << " -dnew- " << vertex_name1 << std::endl;
			}
			else {
				net.setEdgeWeight(vertex_name2,vertex_name1,net.getEdgeWeight(vertex_name2,vertex_name1)+1);
				//std::cout << vertex_name2 << " -dex- " << vertex_name1 << " " << net.getEdgeWeight(vertex_name2,vertex_name1) << std::endl;
			}
		}
	}
	//std::cout << "END flattening" << std::endl;
	return net;
}

Network flatten_weighted(const MultiplexNetwork& mnet, const std::set<std::string>& active_networks) {
	std::set<network_id> net_ids;
	for (std::string name: active_networks) {
		net_ids.insert(mnet.getNetworkId(name));
	}
	return flatten_weighted(mnet, net_ids);
}

