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

std::set<network_id> MultilayerNetwork::getNetworks() const {
	std::set<network_id> networks;
	for (long v=0; v<getNumNetworks(); v++) {
		networks.insert(v);
	}
	return networks;
}

std::set<std::string> MultilayerNetwork::getNetworkNames() const {
	std::set<std::string> networkNames;
	for (std::pair<std::string,network_id> name: network_name_to_id) {
		networkNames.insert(name.first);
	}
	return networkNames;
}

std::set<global_vertex_id> MultilayerNetwork::getVertexes() const {
	std::set<global_vertex_id> vertexes;
	for (int network = 0; network < getNumNetworks(); network++) {
		std::set<vertex_id> local_vertexes = networks[network].getVertexes();
		for (vertex_id v: local_vertexes) {
			vertexes.insert(global_vertex_id(v,network));
		}
	}
	return vertexes;
}

std::set<global_edge_id> MultilayerNetwork::getEdges() const {
	std::set<global_edge_id> edges;
	for (int network = 0; network < getNumNetworks(); network++) {
		std::set<edge_id> local_edges = networks[network].getEdges();
		for (edge_id e: local_edges) {
			edges.insert(global_edge_id(e.v1,e.v2,network,e.directed));
		}
	}
	return edges;
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

const Network& MultilayerNetwork::getNetwork(const std::string& network_name) const {
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

///////////////////////////////////////////////////////////////
/// OPERATIONS ON UNDERLYING NETWORKS                      ////
/// (the following methods just call the corresponding     ////
///  methods on the corresponding networks)                ////
///////////////////////////////////////////////////////////////

/*

vertex_id MultilayerNetwork::addVertex(const std::string& network_name, const std::string& vertex_name) {
	return getNetwork(network_name).addVertex(vertex_name);
}
edge_id MultilayerNetwork::addEdge(const std::string& network_name, const std::string& vertex_name1, const std::string& vertex_name2) {
	return getNetwork(network_name).addEdge(vertex_name1,vertex_name2);
}
edge_id MultilayerNetwork::addEdge(const std::string& network_name, const std::string& vertex_name1, const std::string& vertex_name2, double weight) {
	return getNetwork(network_name).addEdge(vertex_name1,vertex_name2,weight);
}
bool MultilayerNetwork::deleteVertex(const std::string& network_name, const std::string& vertex_name) {
	return getNetwork(network_name).deleteVertex(vertex_name);
}
bool MultilayerNetwork::deleteEdge(const std::string& network_name, const std::string& vertex_name1, const std::string& vertex_name2) {
	return getNetwork(network_name).deleteEdge(vertex_name1,vertex_name2);
}
bool MultilayerNetwork::containsVertex(const std::string& network_name, const std::string& vertex_name) const {
	return getNetwork(network_name).containsVertex(vertex_name);
}
bool MultilayerNetwork::containsEdge(const std::string& network_name, const std::string& vertex_name1, const std::string& vertex_name2) const {
	return getNetwork(network_name).containsEdge(vertex_name1,vertex_name2);
}
bool MultilayerNetwork::isDirected(const std::string& network_name) const {
	return getNetwork(network_name).isDirected();
}
bool MultilayerNetwork::isWeighed(const std::string& network_name) const {
	return getNetwork(network_name).isWeighed();
}
bool MultilayerNetwork::isNamed(const std::string& network_name) const {
	return getNetwork(network_name).isNamed();
}
vertex_id MultilayerNetwork::getVertexId(const std::string& network_name, const std::string& vertex_name) const {
	return getNetwork(network_name).getVertexId(vertex_name);
}
long MultilayerNetwork::getNumVertexes(const std::string& network_name) const {
	return getNetwork(network_name).getNumVertexes();
}
long MultilayerNetwork::getNumEdges(const std::string& network_name) const {
	return getNetwork(network_name).getNumEdges();
}
double MultilayerNetwork::getEdgeWeight(const std::string& network_name, const std::string& vertex_name1, const std::string& vertex_name2) const {
	return getNetwork(network_name).getEdgeWeight(vertex_name1,vertex_name2);
}
void MultilayerNetwork::setEdgeWeight(const std::string& network_name, const std::string& vertex_name1, const std::string& vertex_name2, double weight) {
	getNetwork(network_name).setEdgeWeight(vertex_name1,vertex_name2,weight);
}
bool MultilayerNetwork::hasVertexAttribute(const std::string& network_name, const std::string& attribute_name) {
	return getNetwork(network_name).hasVertexAttribute(attribute_name);
}
bool MultilayerNetwork::hasEdgeAttribute(const std::string& network_name, const std::string& attribute_name) {
	return getNetwork(network_name).hasVertexAttribute(attribute_name);
}
void MultilayerNetwork::newStringVertexAttribute(const std::string& network_name, const std::string& attribute_name) {
	getNetwork(network_name).newStringVertexAttribute(attribute_name);
}
std::string MultilayerNetwork::getStringVertexAttribute(const std::string& network_name, const std::string& vertex_name, const std::string& attribute_name) const {
	return getNetwork(network_name).getStringVertexAttribute(vertex_name,attribute_name);
}
void MultilayerNetwork::setStringVertexAttribute(const std::string& network_name, const std::string& vertex_name, const std::string& attribute_name, const std::string& value) {
	getNetwork(network_name).setStringVertexAttribute(vertex_name,attribute_name,value);
}
void MultilayerNetwork::newNumericVertexAttribute(const std::string& network_name, const std::string& attribute_name) {
	getNetwork(network_name).newNumericVertexAttribute(attribute_name);
}
double MultilayerNetwork::getNumericVertexAttribute(const std::string& network_name, const std::string& vertex_name, const std::string& attribute_name) const {
	return getNetwork(network_name).getNumericVertexAttribute(vertex_name,attribute_name);
}
void MultilayerNetwork::setNumericVertexAttribute(const std::string& network_name, const std::string& vertex_name, const std::string& attribute_name, double value) {
	getNetwork(network_name).setNumericVertexAttribute(vertex_name,attribute_name,value);
}
void MultilayerNetwork::newStringEdgeAttribute(const std::string& network_name, const std::string& attribute_name) {
	getNetwork(network_name).newStringEdgeAttribute(attribute_name);
}
std::string MultilayerNetwork::getStringEdgeAttribute(const std::string& network_name, const std::string& vertex_name1, const std::string& vertex_name2, const std::string& attribute_name) const {
	return getNetwork(network_name).getStringEdgeAttribute(vertex_name1,vertex_name2,attribute_name);
}
void MultilayerNetwork::setStringEdgeAttribute(const std::string& network_name, const std::string& vertex_name1, const std::string& vertex_name2, const std::string& attribute_name, const std::string& value) {
	getNetwork(network_name).setStringEdgeAttribute(vertex_name1,vertex_name2,attribute_name,value);
}
void MultilayerNetwork::newNumericEdgeAttribute(const std::string& network_name, const std::string& attribute_name) {
	getNetwork(network_name).newNumericEdgeAttribute(attribute_name);
}
double MultilayerNetwork::getNumericEdgeAttribute(const std::string& network_name, const std::string& vertex_name1, const std::string& vertex_name2, const std::string& attribute_name) const {
	return getNetwork(network_name).getNumericEdgeAttribute(vertex_name1,vertex_name2,attribute_name);
}
void MultilayerNetwork::setNumericEdgeAttribute(const std::string& network_name, const std::string& vertex_name1, const std::string& vertex_name2, const std::string& attribute_name, double value) {
	getNetwork(network_name).setNumericEdgeAttribute(vertex_name1,vertex_name2,attribute_name,value);
}
*/

void print(MultilayerNetwork& mnet) {
	std::cout << "MULTILAYER NETWORK" << std::endl;
	std::cout << " -Number of networks: " << mnet.getNumNetworks() << std::endl;
	std::cout << " -Number of vertexes: " << mnet.getNumVertexes() << std::endl;
	std::cout << " -Number of edges: " << mnet.getNumEdges() << std::endl;
}
