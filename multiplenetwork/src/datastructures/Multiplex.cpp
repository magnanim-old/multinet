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

Multiplex::Multiplex() {}

Multiplex::~Multiplex() {
	// TODO
}

global_identity Multiplex::addIdentity() {
	addIdentities(1);
	return getNumIdentities()-1;
}

void Multiplex::addIdentities(long num_new_identities) {
	for (long i=0; i<num_new_identities; ++i) {
		// map keeping correspondences between global and local vertex identifiers
		std::map<network_id,vertex_id> m;
		global_to_local_id.push_back(m);
	}
}

void Multiplex::map(global_identity gvid, vertex_id lvid, int nid) {
	if (!containsIdentity(gvid)) throw ElementNotFoundException("global identity " + std::to_string(gvid));
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + std::to_string(nid));
	if (!getNetwork(nid).containsVertex(lvid)) throw ElementNotFoundException("local vertex " + std::to_string(lvid));
	if (global_to_local_id[gvid].count(nid)>0) throw DuplicateElementException("global vertex " + std::to_string(gvid) + " in network " + std::to_string(nid));
	// We update the references between global and local identifiers
	global_to_local_id[gvid][nid] = lvid;
	local_to_global_id[global_vertex_id(lvid,nid)] = gvid;
}

vertex_id Multiplex::getVertexId(global_identity gvid, network_id nid) const {
	if (!containsVertex(gvid,nid)) throw ElementNotFoundException("global identity " + std::to_string(gvid) + " in network " + std::to_string(nid));
	return global_to_local_id.at(gvid).at(nid);
}

global_identity Multiplex::getGlobalIdentity(vertex_id lvid, network_id nid) const {
	if (!getNetwork(nid).containsVertex(lvid)) throw ElementNotFoundException("local vertex " + std::to_string(lvid) + " in network " + std::to_string(nid));
	return local_to_global_id.at(global_vertex_id(lvid,nid));
}

long Multiplex::getNumIdentities() const {
	return global_to_local_id.size();
}

bool Multiplex::containsIdentity(global_identity gvid) const {
	// Identities are numbered from 0, therefore existing global vertex ids range in [0,getNumGlobalVertexes()[
	return gvid < getNumIdentities();
}

bool Multiplex::containsVertex(global_identity gvid, network_id nid) const {
	// check if global vertex exists
	if (!containsIdentity(gvid)) return false;
	// now check if it has an associated local vertex on network nid
	// (if nid does not exist, this condition will always be true)
	if (global_to_local_id[gvid].count(nid)==0) return false;
	else return true;
}

global_identity Multiplex::addIdentity(const std::string& name) {
	global_identity new_identity;
	if (identity_name_to_id.count(name)>0) {
		throw DuplicateElementException("vertex " + name);
	}
	else {
		int num_current_identities = getNumIdentities();
		new_identity = addIdentity();
		identity_name_to_id[name] = num_current_identities;
		identity_id_to_name[new_identity] = name;
	}
	return new_identity;
}


std::string Multiplex::getIdentityName(global_identity gid) const {
	if (!containsIdentity(gid)) throw ElementNotFoundException("identity " + std::to_string(gid));
	return identity_id_to_name.at(gid);
}

vertex_id Multiplex::getIdentityFromName(const std::string& identity_name) const {
	if (!containsIdentity(identity_name)) throw ElementNotFoundException("identity " + identity_name);
	return identity_name_to_id.at(identity_name);

}

void Multiplex::map(const std::string& global_identity_name, const std::string& local_vertex_name, const std::string& network_name) {
	if (!containsIdentity(global_identity_name)) throw ElementNotFoundException("global identity " + global_identity_name);
	if (!getNetwork(network_name).containsVertex(local_vertex_name)) throw ElementNotFoundException("local vertex name " + local_vertex_name);
	if (!containsNetwork(network_name)) throw ElementNotFoundException("network " + network_name);
	map(getIdentityFromName(global_identity_name),getNetwork(network_name).getVertexId(local_vertex_name),getNetworkId(network_name));
}

bool Multiplex::containsIdentity(const std::string& global_identity_name) const {
	return identity_name_to_id.count(global_identity_name);
}

bool Multiplex::containsVertex(const std::string& global_identity_name, const std::string& network_name) const {
	global_identity global_id = getIdentityFromName(global_identity_name);
	network_id net = getNetworkId(network_name);
	return containsVertex(global_id,net);
}

std::string Multiplex::getVertexName(const std::string& global_vertex_name, const std::string& network_name) const {
	global_identity global_id = getIdentityFromName(global_vertex_name);
	network_id net = getNetworkId(network_name);
	vertex_id local_id = getVertexId(global_id, net);
	return getNetwork(net).getVertexName(local_id);
}

std::string Multiplex::getGlobalIdentityName(const std::string& local_vertex_name, const std::string& network_name) const {
	network_id net = getNetworkId(network_name);
	vertex_id local_id = getNetwork(net).getVertexId(local_vertex_name);
	global_identity global_id = getGlobalIdentity(local_id, net);
	return getIdentityName(global_id);
}

void print(Multiplex& mnet) {
	std::cout << "*MULTIPLE NETWORK*\n";
	std::cout << "Number of global identities: " << mnet.getNumIdentities() << "\n";
	std::cout << "Number of networks: " << mnet.getNumNetworks() << "\n";
	std::cout << "Number of vertexes: " << mnet.getNumVertexes() << "\n";
	std::cout << "Number of edges: " << mnet.getNumEdges() << "\n";
}

