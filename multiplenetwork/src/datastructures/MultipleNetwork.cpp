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

global_vertex_id MultipleNetwork::addVertex() {
	addVertexes(1);
	return getNumVertexes()-1;
}

void MultipleNetwork::addVertexes(long num_new_vertexes) {
	for (long i=0; i<num_new_vertexes; ++i) {
		// map keeping correspondences between global and local vertex identifiers
		std::map<network_id,vertex_id> m;
		global_to_local_id.push_back(m);
	}
}

network_id MultipleNetwork::addNetwork(Network& net) {
	int num_current_networks = getNumNetworks();
	networks.resize(num_current_networks+1);
	networks[num_current_networks] = net;
	// map keeping correspondences between local and global vertex identifiers
	std::map<vertex_id,global_vertex_id> m;
	local_to_global_id.push_back(m);
	return getNumNetworks()-1;
}

void MultipleNetwork::getNetworks(std::set<network_id>& networks) {
	for (long v=0; v<getNumNetworks(); v++) {
		networks.insert(v);
	}
}

void MultipleNetwork::getVertexes(std::set<global_vertex_id>& vertexes) {
	for (long v=0; v<getNumVertexes(); v++) {
		vertexes.insert(v);
	}
}

void MultipleNetwork::getEdges(std::set<global_edge_id>& edges) {
	for (int network = 0; network < getNumNetworks(); network++) {
		std::set<vertex_id> vertexes;
		getNetwork(network)->getVertexes(vertexes);
		for (std::set<vertex_id>::iterator from_iterator = vertexes.begin(); from_iterator != vertexes.end(); from_iterator++) {
			vertex_id local_from = *from_iterator;
			std::set<vertex_id> out_neighbors;
			getNetwork(network)->getOutNeighbors(local_from, out_neighbors);
			for (std::set<vertex_id>::iterator to_iterator = out_neighbors.begin(); to_iterator != out_neighbors.end(); to_iterator++) {
						vertex_id local_to = *to_iterator;
						edges.insert(global_edge_id(getGlobalVertexId(local_from,network),getGlobalVertexId(local_to,network),getNetwork(network)->isDirected(),network));
			}
		}
	}
}

void MultipleNetwork::map(global_vertex_id gvid, vertex_id lvid, int nid) {
	if (!containsVertex(gvid)) throw ElementNotFoundException("global vertex " + std::to_string(gvid));
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + std::to_string(nid));
	if (!getNetwork(nid)->containsVertex(lvid)) throw ElementNotFoundException("local vertex " + std::to_string(lvid));
	if (global_to_local_id[gvid].count(nid)>0) throw DuplicateElementException("global vertex " + std::to_string(gvid) + " in network " + std::to_string(nid));
	// We update the references between global and local identifiers
	global_to_local_id[gvid][nid] = lvid;
	local_to_global_id[nid][lvid] = gvid;
}

Network* MultipleNetwork::getNetwork(network_id nid) {
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + std::to_string(nid));
	return &(networks[nid]);
}

vertex_id MultipleNetwork::getLocalVertexId(global_vertex_id gvid, network_id nid) {
	if (!containsVertex(gvid,nid)) throw ElementNotFoundException("global vertex " + std::to_string(gvid) + " in network " + std::to_string(nid));
	return global_to_local_id[gvid][nid];
}

global_vertex_id MultipleNetwork::getGlobalVertexId(vertex_id lvid, network_id nid) {
	if (!getNetwork(nid)->containsVertex(lvid)) throw ElementNotFoundException("local vertex " + std::to_string(lvid) + " in network " + std::to_string(nid));
	return local_to_global_id[nid][lvid];
}

int MultipleNetwork::getNumNetworks() {
	return networks.size();
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

bool MultipleNetwork::containsVertex(global_vertex_id gvid) {
	// Vertexes are numbered from 0, therefore existing global vertex ids range in [0,getNumGlobalVertexes()[
	return gvid < getNumVertexes();
}

bool MultipleNetwork::containsNetwork(network_id nid) {
	// Networks are numbered from 0, therefore existing network ids range in [0,getNumNetworks()[
	return nid < getNumNetworks();
}

bool MultipleNetwork::containsVertex(global_vertex_id gvid, network_id nid) {
	// check if global vertex exists
	if (!containsVertex(gvid)) return false;
	// now check if it has an associated local vertex on network nid
	// (if nid does not exist, this condition will always be true)
	if (global_to_local_id[gvid].count(nid)==0) return false;
	else return true;
}

global_vertex_id MultipleNetwork::addVertex(std::string name) {
	global_vertex_id new_vertex_id;
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

network_id MultipleNetwork::getNetworkId(std::string network_name) {
	if (network_name_to_id.count(network_name)==0) throw ElementNotFoundException("network " + network_name);
	return network_name_to_id[network_name];
}

std::string MultipleNetwork::getVertexName(global_vertex_id gvid) {
	if (!containsVertex(gvid)) throw ElementNotFoundException("global vertex id " + std::to_string(gvid));
	return vertex_id_to_name[gvid];
}

global_vertex_id MultipleNetwork::getVertexId(std::string global_vertex_name) {
	if (!containsVertex(global_vertex_name)) throw ElementNotFoundException("global vertex name " + global_vertex_name);
	return vertex_name_to_id[global_vertex_name];
}

bool MultipleNetwork::containsVertex(std::string global_vertex_name) {
	return vertex_name_to_id.count(global_vertex_name);
}

bool MultipleNetwork::containsVertex(std::string global_vertex_name, std::string network_name) {
	global_vertex_id global_id = getVertexId(global_vertex_name);
	network_id net = getNetworkId(network_name);
	return containsVertex(global_id,net);
}


std::string MultipleNetwork::getLocalVertexName(std::string global_vertex_name, std::string network_name) {
	global_vertex_id global_id = getVertexId(global_vertex_name);
	network_id net = getNetworkId(network_name);
	vertex_id local_id = getLocalVertexId(global_id, net);
	return getNetwork(net)->getVertexName(local_id);
}

std::string MultipleNetwork::getGlobalVertexName(std::string local_vertex_name, std::string network_name) {
	network_id net = getNetworkId(network_name);
	vertex_id local_id = getNetwork(net)->getVertexId(local_vertex_name);
	global_vertex_id global_id = getGlobalVertexId(local_id, net);
	return getVertexName(global_id);
}

bool MultipleNetwork::containsNetwork(std::string network_name) {
	return network_name_to_id.count(network_name)>0;
}

Network* MultipleNetwork::getNetwork(std::string network_name) {
	if (!containsNetwork(network_name)) throw ElementNotFoundException("network " + network_name);
	return &(networks[network_name_to_id[network_name]]);
}

void print(MultipleNetwork& mnet) {
	std::cout << "*MULTIPLE NETWORK*\n";
	std::cout << "Number of vertexes: " << mnet.getNumVertexes() << "\n";
	std::cout << "Number of edges: " << mnet.getNumEdges() << "\n";
	for (int i=0; i<mnet.getNumNetworks(); ++i) {
		std::cout << "Network " << i << "\n";
		print(*mnet.getNetwork(i));
	}
	std::cout << "*END (MULTIPLE NETWORK)*\n";
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
