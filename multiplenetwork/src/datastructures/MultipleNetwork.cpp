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

vertex_id MultipleNetwork::addVertex() {
	addVertexes(1);
	return getNumVertexes()-1;
}

void MultipleNetwork::addVertexes(long num_new_vertexes) {
	for (long i=0; i<num_new_vertexes; ++i) {
		// in map m (stored in global_to_local_id[i]) we will store the local id
		// of each presence of this global vertex in a local network.
		// e.g., after inserting a new local vertex "0" in network "2", corresponding
		// to this global vertex, m[2] will contain "0".
		std::map<network_id,vertex_id> m;
		global_to_local_id.push_back(m);
	}
}

network_id MultipleNetwork::addNetwork(Network& net) {
	int num_current_networks = getNumNetworks();
	graphs.resize(num_current_networks+1);
	graphs[num_current_networks] = net;
	// in map m (stored in local_to_global_id[i]) we will store the global id
	// of every vertex inserted in this network
	std::map<vertex_id,vertex_id> m;
	local_to_global_id.push_back(m);
	return getNumNetworks()-1;
}

void MultipleNetwork::map(vertex_id global_vertex_id, vertex_id local_vertex_id, int nid) {
	if (!containsVertex(global_vertex_id)) throw ElementNotFoundException("global vertex " + std::to_string(global_vertex_id));
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + std::to_string(nid));
	if (!getNetwork(nid)->containsVertex(local_vertex_id)) throw ElementNotFoundException("local vertex " + std::to_string(local_vertex_id));
	if (global_to_local_id[global_vertex_id].count(nid)>0) throw DuplicateElementException("global vertex " + std::to_string(global_vertex_id) + " in network " + std::to_string(nid));
	// We update the references between global and local identifiers
	global_to_local_id[global_vertex_id][nid] = local_vertex_id;
	local_to_global_id[nid][local_vertex_id] = global_vertex_id;
}

Network* MultipleNetwork::getNetwork(int nid) {
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + std::to_string(nid));
	return &(graphs[nid]);
}

long MultipleNetwork::getLocalVertexId(vertex_id global_vertex_id, network_id nid) {
	if (!containsVertex(global_vertex_id,nid)) throw ElementNotFoundException("global vertex " + std::to_string(global_vertex_id) + " in network " + std::to_string(nid));
	return global_to_local_id[global_vertex_id][nid];
}

long MultipleNetwork::getGlobalVertexId(vertex_id local_vertex_id, network_id nid) {
	if (!getNetwork(nid)->containsVertex(local_vertex_id)) throw ElementNotFoundException("local vertex " + std::to_string(local_vertex_id) + " in network " + std::to_string(nid));
	return local_to_global_id[nid][local_vertex_id];
}

int MultipleNetwork::getNumNetworks() {
	return graphs.size();
}

long MultipleNetwork::getNumVertexes() {
	return global_to_local_id.size();
}

long MultipleNetwork::getNumEdges() {
	long number_of_edges = 0;
	for (int net=0; net<getNumNetworks(); net++) {
		number_of_edges += getNetwork(net)->getNumEdges();
	}
	return number_of_edges;
}

bool MultipleNetwork::containsVertex(vertex_id global_vertex_id) {
	// Vertexes are numbered from 0, therefore existing global vertex ids range in [0,getNumGlobalVertexes()[
	return global_vertex_id < getNumVertexes();
}

bool MultipleNetwork::containsNetwork(network_id nid) {
	// Networks are numbered from 0, therefore existing network ids range in [0,getNumNetworks()[
	return nid < getNumNetworks();
}

bool MultipleNetwork::containsVertex(vertex_id global_vertex_id, network_id nid) {
	// check if global vertex exists
	if (!containsVertex(global_vertex_id)) return false;
	// now check if it has an associated local vertex on network nid
	// (if nid does not exist, this condition will always be true)
	if (global_to_local_id[global_vertex_id].count(nid)==0) return false;
	else return true;
}

vertex_id MultipleNetwork::addVertex(std::string name) {
	vertex_id new_vertex_id;
	if (vertex_name_to_id.count(name)>0) {
		throw DuplicateElementException("vertex " + name);
	}
	else {
		int num_current_vertexes = getNumVertexes();
		new_vertex_id = addVertex();
		vertex_name_to_id[name] = num_current_vertexes;
		vertex_id_to_name.push_back(name);
	}
	return new_vertex_id;
}

network_id MultipleNetwork::addNetwork(std::string network_name, Network& net) {
	if (containsNetwork(network_name)) throw DuplicateElementException("network " + network_name);
	network_id new_network_id = addNetwork(net);
	network_name_to_id[network_name] = new_network_id;
	network_id_to_name.push_back(network_name);
	return new_network_id;
}

void MultipleNetwork::map(std::string global_vertex_name, std::string local_vertex_name, std::string network_name) {
	if (!containsVertex(global_vertex_name)) throw ElementNotFoundException("global vertex name " + global_vertex_name);
	if (!getNetwork(network_name)->containsVertex(local_vertex_name)) throw ElementNotFoundException("local vertex name " + local_vertex_name);
	if (!containsNetwork(network_name)) throw ElementNotFoundException("network " + network_name);
	map(getVertexId(global_vertex_name),getNetwork(network_name)->getVertexId(local_vertex_name),getNetworkId(network_name));
}

std::string MultipleNetwork::getNetworkName(network_id nid) {
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + std::to_string(nid));
	return network_id_to_name[nid];
}

int MultipleNetwork::getNetworkId(std::string network_name) {
	if (network_name_to_id.count(network_name)==0) throw ElementNotFoundException("network " + network_name);
	return network_name_to_id[network_name];
}

std::string MultipleNetwork::getVertexName(vertex_id global_vertex_id) {
	if (!containsVertex(global_vertex_id)) throw ElementNotFoundException("global vertex id " + std::to_string(global_vertex_id));
	return vertex_id_to_name[global_vertex_id];
}

long MultipleNetwork::getVertexId(std::string global_vertex_name) {
	if (!containsVertex(global_vertex_name)) throw ElementNotFoundException("global vertex name " + global_vertex_name);
	return vertex_name_to_id[global_vertex_name];
}

bool MultipleNetwork::containsVertex(std::string global_vertex_name) {
	return vertex_name_to_id.count(global_vertex_name);
}

bool MultipleNetwork::containsVertex(std::string global_vertex_name, std::string network_name) {

	vertex_id global_id = getVertexId(global_vertex_name);
	network_id net = getNetworkId(network_name);
	return containsVertex(global_id,net);
}


std::string MultipleNetwork::getLocalVertexName(std::string global_vertex_name, std::string network_name) {
	vertex_id global_id = getVertexId(global_vertex_name);
	network_id net = getNetworkId(network_name);
	vertex_id local_id = getLocalVertexId(global_id, net);
	return getNetwork(net)->getVertexName(local_id);
}

std::string MultipleNetwork::getGlobalVertexName(std::string local_vertex_name, std::string network_name) {
	network_id net = getNetworkId(network_name);
	vertex_id local_id = getNetwork(net)->getVertexId(local_vertex_name);
	vertex_id global_id = getGlobalVertexId(local_id, net);
	return getVertexName(global_id);
}

bool MultipleNetwork::containsNetwork(std::string network_name) {
	return network_name_to_id.count(network_name)>0;
}

Network* MultipleNetwork::getNetwork(std::string network_name) {
	if (!containsNetwork(network_name)) throw ElementNotFoundException("network " + network_name);
	return &(graphs[network_name_to_id[network_name]]);
}

void print(MultipleNetwork& mnet) {
	std::cout << "*Network statistics*\n";
	std::cout << "Number of vertexes: " << mnet.getNumVertexes() << "\n";
	std::cout << "Number of edges: " << mnet.getNumEdges() << "\n";
	for (int i=0; i<mnet.getNumNetworks(); ++i) {
		std::cout << "Network " << i << "\n";
		print(*mnet.getNetwork(i));
	}
}

/**
 * @tested no
 *
void print_debug(MultipleNetwork *mnet) {
	cout << "*Network statistics*\n";
	cout << "Number of vertexes: " << mnet->getNumGlobalVertexes() << "\n";
	for (int i=0; i<mnet->getNumGlobalVertexes(); i++) {
		cout << i << ":" << mnet->getGlobalVertexName(i) << ":" <<
				mnet->getGlobalVertexId(mnet->getGlobalVertexName(i)) << " ";
	}
	cout << "\nNumber of edges: " << mnet->getNumGlobalEdges() << "\n";
	for (int i=0; i<mnet->getNumNetworks(); ++i) {
		cout << "Network " << i << "\n";
		for (int v=0; v<mnet->getNetwork(i)->getNumVertexes(); v++) {
			cout << v << ":" << mnet->getGlobalVertexId(v,i) << ":" <<
			mnet->getGlobalVertexName(mnet->getGlobalVertexId(v,i)) << " ";
		}

		print(*mnet->getNetwork(i));
	}
}*/
