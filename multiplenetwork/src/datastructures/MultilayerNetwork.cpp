/**
 * @class MultipleNetwork
 *
 * @author Matteo Magnani <matteo.magnani@it.uu.se>
 * @date August 2013
 * @version 0.1.0
 *
 * Data structure containing several interdependent networks. In this version the
 * only kind of interdependency is node correspondence: every vertex in one of the networks
 * has a global identifier, and different nodes may share the same global identifier.
 *
 */

#include "datastructures.h"
#include "exceptions.h"
#include "utils.h"
#include <iostream>
#include <sstream>

MultilayerNetwork::MultilayerNetwork() {}

MultilayerNetwork::~MultilayerNetwork() {
	// TODO
}

network_id MultilayerNetwork::addNetwork(const Network& net) {
	int num_current_networks = getNumNetworks();
	networks.resize(num_current_networks+1);
	networks[num_current_networks] = net;
	return getNumNetworks()-1;
}

network_id MultilayerNetwork::addNetwork(const std::string& network_name, const Network& net) {
	if (containsNetwork(network_name)) throw DuplicateElementException("network " + network_name);
	network_id new_network_id = addNetwork(net);
	network_name_to_id[network_name] = new_network_id;
	network_id_to_name.push_back(network_name);
	return new_network_id;
}

void MultilayerNetwork::getNetworks(std::set<network_id>& networks) const {
	for (long v=0; v<getNumNetworks(); v++) {
		networks.insert(v);
	}
}

void MultilayerNetwork::getVertexes(std::set<global_vertex_id>& vertexes) const {
	for (int network = 0; network < getNumNetworks(); network++) {
		std::set<vertex_id> local_vertexes;
		networks[network].getVertexes(local_vertexes);
		for (vertex_id v: local_vertexes) {
			vertexes.insert(global_vertex_id(v,network));
		}
	}
}

void MultilayerNetwork::getEdges(std::set<global_edge_id>& edges) const {
	for (int network = 0; network < getNumNetworks(); network++) {
		std::set<edge_id> local_edges;
		networks[network].getEdges(local_edges);
		for (edge_id e: local_edges) {
			edges.insert(global_edge_id(e.v1,e.v2,network,e.directed));
		}
	}
}


const Network& MultilayerNetwork::getNetwork(network_id nid) const {
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + std::to_string(nid));
	return networks[nid];
}

Network& MultilayerNetwork::getNetwork(network_id nid) {
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + std::to_string(nid));
	return networks[nid];
}

Network& MultilayerNetwork::getNetwork(const std::string& network_name) {
	if (!containsNetwork(network_name)) throw ElementNotFoundException("network " + network_name);
	return networks.at(network_name_to_id.at(network_name));
}

int MultilayerNetwork::getNumNetworks() const {
	return networks.size();
}

long MultilayerNetwork::getNumVertexes() const {
	long number_of_vertexes = 0;
	for (int net=0; net<getNumNetworks(); net++) {
		number_of_vertexes += networks[net].getNumVertexes();
	}
	return number_of_vertexes;
}

long MultilayerNetwork::getNumEdges() const {
	long number_of_edges = 0;
	for (int net=0; net<getNumNetworks(); net++) {
		number_of_edges += networks[net].getNumEdges();
	}
	return number_of_edges;
}

bool MultilayerNetwork::containsNetwork(network_id nid) const {
	// Networks are numbered from 0, therefore existing network ids range in [0,getNumNetworks()[
	return nid < getNumNetworks();
}

std::string MultilayerNetwork::getNetworkName(network_id nid) const {
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + std::to_string(nid));
	return network_id_to_name[nid];
}

network_id MultilayerNetwork::getNetworkId(const std::string& network_name) const {
	if (network_name_to_id.count(network_name)==0) throw ElementNotFoundException("network " + network_name);
	return network_name_to_id.at(network_name);
}

bool MultilayerNetwork::containsNetwork(const std::string& network_name) const {
	return network_name_to_id.count(network_name)>0;
}

void print(MultilayerNetwork& mnet) {
	std::cout << "MULTILAYER NETWORK" << std::endl;
	std::cout << " -Number of networks: " << mnet.getNumNetworks() << std::endl;
	std::cout << " -Number of vertexes: " << mnet.getNumVertexes() << std::endl;
	std::cout << " -Number of edges: " << mnet.getNumEdges() << std::endl;
}
