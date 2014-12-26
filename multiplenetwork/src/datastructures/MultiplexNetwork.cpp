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

MultiplexNetwork::MultiplexNetwork() {}

MultiplexNetwork::~MultiplexNetwork() {
	// TODO
}

entity_id MultiplexNetwork::addGlobalIdentity() {
	addGlobalIdentities(1);
	return getNumGlobalIdentities()-1;
}

std::set<entity_id> MultiplexNetwork::getGlobalIdentities() const {
	std::set<entity_id> identities;
	for (std::pair<entity_id,std::string> p: identity_id_to_name)
		identities.insert(p.first);
	return identities;
}


std::set<std::string> MultiplexNetwork::getGlobalNames() const {
	std::set<std::string> names;
	for (std::pair<std::string,entity_id> p: identity_name_to_id)
		names.insert(p.first);
	return names;
}

void MultiplexNetwork::addGlobalIdentities(long num_new_identities) {
	for (long i=0; i<num_new_identities; ++i) {
		// map keeping correspondences between global and local vertex identifiers
		std::map<network_id,vertex_id> m;
		global_to_local_id.push_back(m);
	}
}

void MultiplexNetwork::map(entity_id gvid, vertex_id lvid, int nid) {
	if (!containsGlobalIdentity(gvid)) throw ElementNotFoundException("global identity " + to_string(gvid));
	if (!containsNetwork(nid)) throw ElementNotFoundException("network " + to_string(nid));
	if (!getNetwork(nid).containsVertex(lvid)) throw ElementNotFoundException("local vertex " + to_string(lvid));
	if (global_to_local_id[gvid].count(nid)>0) throw DuplicateElementException("global vertex " + to_string(gvid) + " in network " + to_string(nid));
	// We update the references between global and local identifiers
	global_to_local_id[gvid][nid] = lvid;
	local_to_global_id[global_vertex_id(lvid,nid)] = gvid;
}

vertex_id MultiplexNetwork::getVertexId(entity_id gvid, network_id nid) const {
	if (!containsVertex(gvid,nid)) throw ElementNotFoundException("global identity " + to_string(gvid) + " in network " + to_string(nid));
	return global_to_local_id.at(gvid).at(nid);
}

entity_id MultiplexNetwork::getGlobalIdentity(vertex_id lvid, network_id nid) const {
	if (!getNetwork(nid).containsVertex(lvid)) throw ElementNotFoundException("local vertex " + to_string(lvid) + " in network " + to_string(nid));
	return local_to_global_id.at(global_vertex_id(lvid,nid));
}

long MultiplexNetwork::getNumGlobalIdentities() const {
	return global_to_local_id.size();
}

bool MultiplexNetwork::containsGlobalIdentity(entity_id gvid) const {
	// Identities are numbered from 0, therefore existing global vertex ids range in [0,getNumGlobalVertexes()[
	return gvid < getNumGlobalIdentities();
}

bool MultiplexNetwork::containsVertex(entity_id gvid, network_id nid) const {
	// check if global vertex exists
	if (!containsGlobalIdentity(gvid)) return false;
	// now check if it has an associated local vertex on network nid
	// (if nid does not exist, this condition will always be true)
	if (global_to_local_id[gvid].count(nid)==0) return false;
	else return true;
}

entity_id MultiplexNetwork::addGlobalName(const std::string& name) {
	entity_id new_identity;
	if (identity_name_to_id.count(name)>0) {
		throw DuplicateElementException("vertex " + name);
	}
	else {
		int num_current_identities = getNumGlobalIdentities();
		new_identity = addGlobalIdentity();
		identity_name_to_id[name] = num_current_identities;
		identity_id_to_name[new_identity] = name;
	}
	return new_identity;
}


std::string MultiplexNetwork::getGlobalName(entity_id gid) const {
	if (!containsGlobalIdentity(gid)) throw ElementNotFoundException("identity " + to_string(gid));
	return identity_id_to_name.at(gid);
}

vertex_id MultiplexNetwork::getGlobalIdentity(const std::string& identity_name) const {
	if (!containsGlobalName(identity_name)) throw ElementNotFoundException("identity " + identity_name);
	return identity_name_to_id.at(identity_name);

}

void MultiplexNetwork::map(const std::string& global_identity_name, const std::string& local_vertex_name, const std::string& network_name) {
	if (!containsGlobalName(global_identity_name)) throw ElementNotFoundException("global identity " + global_identity_name);
	if (!getNetwork(network_name).containsVertex(local_vertex_name)) throw ElementNotFoundException("local vertex name " + local_vertex_name);
	if (!containsNetwork(network_name)) throw ElementNotFoundException("network " + network_name);
	map(getGlobalIdentity(global_identity_name),getNetwork(network_name).getVertexId(local_vertex_name),getNetworkId(network_name));
}

bool MultiplexNetwork::containsGlobalName(const std::string& global_identity_name) const {
	return identity_name_to_id.count(global_identity_name);
}

bool MultiplexNetwork::containsVertex(const std::string& global_identity_name, const std::string& network_name) const {
	entity_id global_id = getGlobalIdentity(global_identity_name);
	network_id net = getNetworkId(network_name);
	return containsVertex(global_id,net);
}

std::string MultiplexNetwork::getVertexName(const std::string& global_vertex_name, const std::string& network_name) const {
	entity_id global_id = getGlobalIdentity(global_vertex_name);
	network_id net = getNetworkId(network_name);
	vertex_id local_id = getVertexId(global_id, net);
	return getNetwork(net).getVertexName(local_id);
}

std::string MultiplexNetwork::getGlobalName(const std::string& local_vertex_name, const std::string& network_name) const {
	network_id net = getNetworkId(network_name);
	vertex_id local_id = getNetwork(net).getVertexId(local_vertex_name);
	entity_id global_id = getGlobalIdentity(local_id, net);
	return getGlobalName(global_id);
}


bool MultiplexNetwork::hasAttribute(const std::string& attribute_name) const {
	return (hasStringAttribute(attribute_name) || hasNumericAttribute(attribute_name));
}

bool MultiplexNetwork::hasStringAttribute(const std::string& attribute_name) const {
	return (string_attribute.count(attribute_name)>0);
}

bool MultiplexNetwork::hasNumericAttribute(const std::string& attribute_name) const {
	return (numeric_attribute.count(attribute_name)>0);
}

void MultiplexNetwork::newNumericAttribute(const std::string& attribute_name) {
	if (numeric_attribute.count(attribute_name)>0) throw DuplicateElementException("Attribute " + attribute_name);
	numeric_attribute[attribute_name] = std::map<vertex_id,double>();
}

void MultiplexNetwork::newStringAttribute(const std::string& attribute_name) {
	if (string_attribute.count(attribute_name)>0) throw DuplicateElementException("Attribute " + attribute_name);
	string_attribute[attribute_name] = std::map<vertex_id,std::string>();
}

double MultiplexNetwork::getNumericAttribute(const entity_id& gid, const std::string& attribute_name) const {
	if (numeric_attribute.count(attribute_name)==0) throw ElementNotFoundException("Attribute " + attribute_name);
	if (numeric_attribute.at(attribute_name).count(gid)==0)  throw ElementNotFoundException("No attribute value for attribute " + attribute_name + " on global name " + to_string(gid));
	return numeric_attribute.at(attribute_name).at(gid);
}

double MultiplexNetwork::getNumericAttribute(const std::string& global_name, const std::string& attribute_name) const {
	if (!containsGlobalName(global_name)) throw ElementNotFoundException("Vertex " + global_name);
	return getNumericAttribute(identity_name_to_id.at(global_name), attribute_name);
}

std::string MultiplexNetwork::getStringAttribute(const entity_id& gid, const std::string& attribute_name) const {
	if (string_attribute.count(attribute_name)==0) throw ElementNotFoundException("Attribute " + attribute_name);
	if (string_attribute.at(attribute_name).count(gid)==0)  throw ElementNotFoundException("No attribute value for attribute " + attribute_name + " on global name " + to_string(gid));
	return string_attribute.at(attribute_name).at(gid);
}

std::string MultiplexNetwork::getStringAttribute(const std::string& global_name, const std::string& attribute_name) const {
	if (!containsGlobalName(global_name)) throw ElementNotFoundException("Global name " + global_name);
	return getStringAttribute(identity_name_to_id.at(global_name), attribute_name);
}

void MultiplexNetwork::setNumericAttribute(const entity_id& gid, const std::string& attribute_name, double val) {
	if (numeric_attribute.count(attribute_name)==0) throw ElementNotFoundException("Attribute " + attribute_name);
	numeric_attribute.at(attribute_name)[gid] = val;
}

void MultiplexNetwork::setNumericAttribute(const std::string& global_name, const std::string& attribute_name, double val) {
	setNumericAttribute(identity_name_to_id[global_name], attribute_name, val);
}

void MultiplexNetwork::setStringAttribute(const entity_id& gid, const std::string& attribute_name, const std::string& val) {
	if (string_attribute.count(attribute_name)==0) throw ElementNotFoundException("Attribute " + attribute_name);
	string_attribute.at(attribute_name)[gid] = val;
}

void MultiplexNetwork::setStringAttribute(const std::string& global_name, const std::string& attribute_name, const std::string& val) {
	setStringAttribute(identity_name_to_id[global_name], attribute_name, val);
}


std::set<std::string> MultiplexNetwork::getNumericAttributes() const {
	std::set<std::string> res;
	std::map<std::string, std::map<vertex_id, double> >::const_iterator pair;
	for (pair = numeric_attribute.begin(); pair != numeric_attribute.end(); ++pair)  {
			res.insert((*pair).first);
	}
	return res;
}

std::set<std::string> MultiplexNetwork::getStringAttributes() const {
	std::set<std::string> res;
	std::map<std::string, std::map<vertex_id, std::string> >::const_iterator pair;
	for (pair = string_attribute.begin(); pair != string_attribute.end(); ++pair)  {
		res.insert((*pair).first);
	}
	return res;
}

bool MultiplexNetwork::deleteVertex(entity_id gid, network_id nid) {
	if (!containsGlobalIdentity(gid)) return false;
	if (!containsNetwork(nid)) return false;
	if (!containsVertex(gid,nid)) return false;
	vertex_id lvid = getVertexId(gid,nid);
	getNetwork(nid).deleteVertex(lvid);
	// We update the references between global and local identifiers
	global_to_local_id[gid].erase(nid);
	local_to_global_id.erase(global_vertex_id(lvid,nid));
	return true;
}

bool MultiplexNetwork::deleteVertex(const std::string& global_name, const std::string& network) {
	return deleteVertex(getGlobalIdentity(global_name), getNetworkId(network));
}

std::ostream& operator<<(std::ostream &strm, const MultiplexNetwork& mnet) {
	strm << "multiplex (identities: " << mnet.getNumGlobalIdentities();
	strm << ", networks: " << mnet.getNumNetworks();
	strm << ", vertexes: " << mnet.getNumVertexes();
	strm << ", edges: " << mnet.getNumEdges() << ")";
	return strm;
}

