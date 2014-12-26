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

MultipleNetwork::MultipleNetwork() {}

MultipleNetwork::~MultipleNetwork() {
	// TODO
}

network_id MultipleNetwork::addNetwork(const Network& net) {
	int num_current_networks = getNumNetworks();
	networks.resize(num_current_networks+1);
	networks[num_current_networks] = net;
	int new_network_id = getNumNetworks()-1;
	std::string network_name = to_string(new_network_id); // default name
	network_name_to_id[network_name] = new_network_id;
	network_id_to_name.push_back(network_name);
	return new_network_id;
}

network_id MultipleNetwork::addNetwork(const std::string& network_name, Network& net) {
	if (containsNetwork(network_name)) throw DuplicateElementException("network " + network_name);
	int num_current_networks = getNumNetworks();
	networks.resize(num_current_networks+1);
	networks[num_current_networks] = net;
	int new_network_id = getNumNetworks()-1;
	network_name_to_id[network_name] = new_network_id;
	network_id_to_name.push_back(network_name);
	return new_network_id;
}

std::set<network_id> MultipleNetwork::getNetworks() const {
	std::set<network_id> networks;
	for (long v=0; v<getNumNetworks(); v++) {
		networks.insert(v);
	}
	return networks;
}

std::set<std::string> MultipleNetwork::getNetworkNames() const {
	std::set<std::string> networkNames;
	for (std::pair<std::string,network_id> name: network_name_to_id) {
		networkNames.insert(name.first);
	}
	return networkNames;
}

std::set<global_vertex_id> MultipleNetwork::getVertexes() const {
	std::set<global_vertex_id> vertexes;
	for (int network = 0; network < getNumNetworks(); network++) {
		std::set<vertex_id> local_vertexes = networks[network].getVertexes();
		for (vertex_id v: local_vertexes) {
			vertexes.insert(global_vertex_id(v,network));
		}
	}
	return vertexes;
}

std::set<global_edge_id> MultipleNetwork::getEdges() const {
	std::set<global_edge_id> edges;
	for (int network = 0; network < getNumNetworks(); network++) {
		std::set<edge_id> local_edges = networks[network].getEdges();
		for (edge_id e: local_edges) {
			edges.insert(global_edge_id(e.v1,e.v2,network,e.directed));
		}
	}
	return edges;
}

const Network& MultipleNetwork::getNetwork(network_id nid) const {
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + to_string(nid));
	return networks[nid];
}

Network& MultipleNetwork::getNetwork(network_id nid) {
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + to_string(nid));
	return networks[nid];
}

Network& MultipleNetwork::getNetwork(const std::string& network_name) {
	if (!containsNetwork(network_name)) throw ElementNotFoundException("network " + network_name);
	return networks.at(network_name_to_id.at(network_name));
}

const Network& MultipleNetwork::getNetwork(const std::string& network_name) const {
	if (!containsNetwork(network_name)) throw ElementNotFoundException("network " + network_name);
	return networks.at(network_name_to_id.at(network_name));
}

int MultipleNetwork::getNumNetworks() const {
	return networks.size();
}

long MultipleNetwork::getNumVertexes() const {
	long number_of_vertexes = 0;
	for (int net=0; net<getNumNetworks(); net++) {
		number_of_vertexes += networks[net].getNumVertexes();
	}
	return number_of_vertexes;
}

long MultipleNetwork::getNumEdges() const {
	long number_of_edges = 0;
	for (int net=0; net<getNumNetworks(); net++) {
		number_of_edges += networks[net].getNumEdges();
	}
	return number_of_edges;
}

bool MultipleNetwork::containsNetwork(network_id nid) const {
	// Networks are numbered from 0, therefore existing network ids range in [0,getNumNetworks()[
	return nid < getNumNetworks();
}

std::string MultipleNetwork::getNetworkName(network_id nid) const {
	// TODO problem if network has been added without a name
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + to_string(nid));
	return network_id_to_name[nid];
}

network_id MultipleNetwork::getNetworkId(const std::string& network_name) const {
	if (network_name_to_id.count(network_name)==0) throw ElementNotFoundException("network " + network_name);
	return network_name_to_id.at(network_name);
}

bool MultipleNetwork::containsNetwork(const std::string& network_name) const {
	return network_name_to_id.count(network_name)>0;
}

std::ostream& operator<<(std::ostream &strm, const MultipleNetwork& mnet) {
	strm << "multilayer network (";
	strm << ", networks: " << mnet.getNumNetworks();
	strm << ", vertexes: " << mnet.getNumVertexes();
	strm << ", edges: " << mnet.getNumEdges() << ")";
	return strm;
}
