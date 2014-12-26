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

const std::string SEP = "|";

Network flatten_weighted(const MultiplexNetwork& mnet, const std::set<network_id>& active_networks, bool force_directed, bool force_all_actors) {
	// verify if there are directed networks: if yes, the resulting flattened network is also directed
	bool directed = force_directed;
	if (!force_directed) {
	for (network_id network: active_networks) {
		if (mnet.getNetwork(network).isDirected()) {
			directed = true;
			continue;
		}
	}
	}
	Network net(true,directed,true);

	// Add global names
	if (force_all_actors) {
	std::set<std::string> vertexNames = mnet.getGlobalNames();
	for (std::string name: vertexNames) {
		net.addVertex(name);
	}
	}
	else {
	for (network_id network: active_networks) {
		for (vertex_id vid: mnet.getNetwork(network).getVertexes()) {
			std::string name = mnet.getGlobalName(mnet.getGlobalIdentity(vid,network));
			if (!net.containsVertex(name))
				net.addVertex(name);
		}
	}
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

// WARNING: returns all actors
Network flatten_multi(const MultiplexNetwork& mnet, const std::set<network_id>& active_networks, bool force_directed, bool force_all_actors) {
	// verify if there are directed networks: if yes, the resulting flattened network is also directed
	bool directed = force_directed;
	if (!force_directed) {
	for (network_id network: active_networks) {
		if (mnet.getNetwork(network).isDirected()) {
			directed = true;
			continue;
		}
	}
	}
	Network net(true,directed,true);
	net.newStringVertexAttribute("_networks");
	net.newStringEdgeAttribute("_networks");


	std::set<identity> ids = mnet.getGlobalIdentities();
	for (identity id: ids) {
		std::string name = mnet.getGlobalName(id);
		vertex_id vid = net.addVertex(name);
		std::string networks = "";
		for (network_id network: active_networks) {
			networks = networks + SEP + mnet.getNetworkName(network);
		}
		net.setStringVertexAttribute(vid,"_networks",networks);
	}

	//std::cout << "BEGIN flattening" << std::endl;

	std::set<global_edge_id> edges = mnet.getEdges();
	//std::cout << edges.size() << std::endl;
	for (global_edge_id e: edges) {
		// not very efficient implementation when only a few networks are flattened...
		if (active_networks.count(e.network)==0) continue;

		std::string vertex_name1 = mnet.getGlobalName(mnet.getGlobalIdentity(e.v1,e.network));
		std::string vertex_name2 = mnet.getGlobalName(mnet.getGlobalIdentity(e.v2,e.network));
		std::string network_name = mnet.getNetworkName(e.network);
		if (!net.containsEdge(vertex_name1,vertex_name2)) {
			net.addEdge(vertex_name1,vertex_name2);
			net.setStringEdgeAttribute(vertex_name1,vertex_name2,"_networks",network_name);
		}
		else {
			std::string in_networks = net.getStringEdgeAttribute(vertex_name1,vertex_name2,"_networks");
			net.setStringEdgeAttribute(vertex_name1,vertex_name2,"_networks",in_networks + SEP + network_name);
			//std::cout << vertex_name1 << " -ex- " << vertex_name2 << " " << net.getEdgeWeight(vertex_name1,vertex_name2) << std::endl;
		}
		// if the resulting network is directed, undirected edges must be inserted as two directed ones
		if (!net.containsEdge(vertex_name2,vertex_name1)) {
			net.addEdge(vertex_name2,vertex_name1);
			net.setStringEdgeAttribute(vertex_name2,vertex_name1,"_networks",network_name);
		}
		else {
			std::string in_networks = net.getStringEdgeAttribute(vertex_name2,vertex_name1,"_networks");
			net.setStringEdgeAttribute(vertex_name2,vertex_name1,"_networks",in_networks + SEP + network_name);
			//std::cout << vertex_name1 << " -ex- " << vertex_name2 << " " << net.getEdgeWeight(vertex_name1,vertex_name2) << std::endl;
		}
	}
	//std::cout << "END flattening" << std::endl;
	return net;
}


Network flatten_or(const MultiplexNetwork& mnet, const std::set<network_id>& active_networks, bool force_directed, bool force_all_actors) {
	// verify if there are directed networks: if yes, the resulting flattened network is also directed
	bool directed = force_directed;
	if (!force_directed) {
	for (network_id network: active_networks) {
		if (mnet.getNetwork(network).isDirected()) {
			directed = true;
			continue;
		}
	}
	}
	Network net(true,directed,false);


	// Add global names
	if (force_all_actors) {
	std::set<std::string> vertexNames = mnet.getGlobalNames();
	for (std::string name: vertexNames) {
		net.addVertex(name);
	}
	}
	else {
	for (network_id network: active_networks) {
		for (vertex_id vid: mnet.getNetwork(network).getVertexes()) {
			std::string name = mnet.getGlobalName(mnet.getGlobalIdentity(vid,network));
			if (!net.containsVertex(name))
				net.addVertex(name);
		}
	}
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
			net.addEdge(vertex_name1,vertex_name2);
			//std::cout << vertex_name1 << " -new- " << vertex_name2 << " " << 1 << std::endl;
		}
		// if the resulting network is directed, undirected edges must be inserted as two directed ones
		if (directed && !e.directed) {
			if (!net.containsEdge(vertex_name2,vertex_name1)) {
				net.addEdge(vertex_name2,vertex_name1);
				//std::cout << vertex_name2 << " -dnew- " << vertex_name1 << std::endl;
			}
		}
	}
	//std::cout << "END flattening" << std::endl;
	return net;
}

Network flatten_weighted(const MultiplexNetwork& mnet, const std::set<std::string>& active_networks, bool force_directed, bool force_all_actors) {
	std::set<network_id> net_ids;
	for (std::string name: active_networks) {
		net_ids.insert(mnet.getNetworkId(name));
	}
	return flatten_weighted(mnet, net_ids, force_directed, force_all_actors);
}

Network flatten_multi(const MultiplexNetwork& mnet, const std::set<std::string>& active_networks, bool force_directed, bool force_all_actors) {
	std::set<network_id> net_ids;
	for (std::string name: active_networks) {
		net_ids.insert(mnet.getNetworkId(name));
	}
	return flatten_multi(mnet, net_ids, force_directed, force_all_actors);
}

Network flatten_or(const MultiplexNetwork& mnet, const std::set<std::string>& active_networks, bool force_directed, bool force_all_actors) {
	std::set<network_id> net_ids;
	for (std::string name: active_networks) {
		net_ids.insert(mnet.getNetworkId(name));
	}
	return flatten_or(mnet, net_ids, force_directed, force_all_actors);
}

