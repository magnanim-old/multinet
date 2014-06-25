/*
 * degree.cpp
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "measures.h"
#include <iostream>

std::set<global_identity> out_neighbors(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks) {
	std::set<global_identity> res;
	for (network_id net: active_networks) {
		if (!mnet.containsVertex(identity,net)) continue;
		std::set<vertex_id> neighbours = mnet.getNetwork(net).getOutNeighbors(mnet.getVertexId(identity,net));
		for (vertex_id vid: neighbours)
			res.insert(mnet.getGlobalIdentity(vid,net));
	}
	return res;
}

std::set<std::string> out_neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	std::set<std::string> res;
	for (std::string net: active_networks) {
		if (!mnet.containsVertex(global_name,net)) continue;
		std::set<std::string> neighbours = mnet.getNetwork(net).getOutNeighbors(mnet.getVertexName(global_name,net));
		for (std::string vid: neighbours)
			res.insert(mnet.getGlobalName(vid,net));
	}
	return res;
}

std::set<global_identity> out_neighbors(const MultiplexNetwork& mnet, global_identity identity, network_id network) {
	std::set<global_identity> res;
	if (!mnet.containsVertex(identity,network)) return res;
	std::set<vertex_id> neighbours = mnet.getNetwork(network).getOutNeighbors(mnet.getVertexId(identity,network));
	for (vertex_id vid: neighbours)
		res.insert(mnet.getGlobalIdentity(vid,network));
	return res;
}

std::set<std::string> out_neighbors(const MultiplexNetwork& mnet, const std::string& name, const std::string& network_name) {
	std::set<std::string> res;
	if (!mnet.containsVertex(name,network_name)) return res;
	std::set<std::string> neighbours = mnet.getNetwork(network_name).getOutNeighbors(mnet.getVertexName(name,network_name));
	for (std::string neigh: neighbours)
		res.insert(mnet.getGlobalName(neigh,network_name));
	return res;
}


std::set<global_identity> in_neighbors(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks) {
	std::set<global_identity> res;
	for (network_id net: active_networks) {
		if (!mnet.containsVertex(identity,net)) continue;
		std::set<vertex_id> neighbours = mnet.getNetwork(net).getInNeighbors(mnet.getVertexId(identity,net));
		for (vertex_id vid: neighbours)
			res.insert(mnet.getGlobalIdentity(vid,net));
	}
	return res;
}

std::set<std::string> in_neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	std::set<std::string> res;
	for (std::string network_name: active_networks) {
		if (!mnet.containsVertex(global_name,network_name)) continue;
		std::set<std::string> neighbours = mnet.getNetwork(network_name).getInNeighbors(mnet.getVertexName(global_name,network_name));
		for (std::string neigh: neighbours)
			res.insert(mnet.getGlobalName(neigh,network_name));
	}
	return res;
}

std::set<global_identity> in_neighbors(const MultiplexNetwork& mnet, global_identity identity, network_id network) {
	std::set<global_identity> res;
	if (!mnet.containsVertex(identity,network)) return res;
	std::set<vertex_id> neighbours = mnet.getNetwork(network).getInNeighbors(mnet.getVertexId(identity,network));
	for (vertex_id vid: neighbours)
		res.insert(mnet.getGlobalIdentity(vid,network));
	return res;
}

std::set<std::string> in_neighbors(const MultiplexNetwork& mnet, const std::string& name, const std::string& network_name) {
	std::set<std::string> res;
	if (!mnet.containsVertex(name,network_name)) return res;
	std::set<std::string> neighbours = mnet.getNetwork(network_name).getInNeighbors(mnet.getVertexName(name,network_name));
	for (std::string neigh: neighbours)
		res.insert(mnet.getGlobalName(neigh,network_name));
	return res;
}


std::set<global_identity> neighbors(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks) {
	std::set<global_identity> res;
	for (network_id net: active_networks) {
		if (!mnet.containsVertex(identity,net)) continue;
		std::set<vertex_id> neighbours = mnet.getNetwork(net).getNeighbors(mnet.getVertexId(identity,net));
		for (vertex_id vid: neighbours)
			res.insert(mnet.getGlobalIdentity(vid,net));
	}
	return res;
}

std::set<std::string> neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	std::set<std::string> res;
	for (std::string network_name: active_networks) {
		if (!mnet.containsVertex(global_name,network_name)) continue;
		std::set<std::string> neighbours = mnet.getNetwork(network_name).getNeighbors(mnet.getVertexName(global_name,network_name));
		for (std::string neigh: neighbours)
			res.insert(mnet.getGlobalName(neigh,network_name));
	}
	return res;
}

std::set<global_identity> neighbors(const MultiplexNetwork& mnet, global_identity identity, network_id network) {
	std::set<global_identity> res;
	if (!mnet.containsVertex(identity,network)) return res;
	std::set<vertex_id> neighbours = mnet.getNetwork(network).getNeighbors(mnet.getVertexId(identity,network));
	for (vertex_id vid: neighbours)
		res.insert(mnet.getGlobalIdentity(vid,network));
	return res;
}

std::set<std::string> neighbors(const MultiplexNetwork& mnet, const std::string& name, const std::string& network_name) {
	std::set<std::string> res;
	if (!mnet.containsVertex(name,network_name)) return res;
	std::set<std::string> neighbours = mnet.getNetwork(network_name).getNeighbors(mnet.getVertexName(name,network_name));
	for (std::string neigh: neighbours)
		res.insert(mnet.getGlobalName(neigh,network_name));
	return res;
}

///////////////////////////////////

std::set<global_identity> out_xneighbors(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks) {
	std::set<network_id> other_networks = mnet.getNetworks();
	for (network_id net: active_networks)
			other_networks.erase(net);

	std::set<global_identity> neighbors_on_active_networks = out_neighbors(mnet, identity, active_networks);
	std::set<global_identity> neighbors_on_other_networks = out_neighbors(mnet, identity, other_networks);

	for (global_identity neighbor: neighbors_on_other_networks)
		neighbors_on_active_networks.erase(neighbor);
	return neighbors_on_active_networks;
}

std::set<std::string> out_xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	std::set<std::string> other_networks = mnet.getNetworkNames();
	for (std::string net: active_networks)
		other_networks.erase(net);

	std::set<std::string> neighbors_on_active_networks = out_neighbors(mnet, global_name, active_networks);
	std::set<std::string> neighbors_on_other_networks = out_neighbors(mnet, global_name, other_networks);

	for (std::string neighbor: neighbors_on_other_networks)
		neighbors_on_active_networks.erase(neighbor);
	return neighbors_on_active_networks;
}

std::set<global_identity> out_xneighbors(const MultiplexNetwork& mnet, global_identity identity, network_id network) {
	std::set<network_id> active_networks;
	active_networks.insert(network);
	return out_xneighbors(mnet, identity, active_networks);
}

std::set<std::string> out_xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name) {
	std::set<std::string> active_networks;
	active_networks.insert(network_name);
	return out_xneighbors(mnet, global_name, active_networks);
}

std::set<global_identity> in_xneighbors(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks) {
	std::set<network_id> other_networks = mnet.getNetworks();
	for (network_id net: active_networks)
			other_networks.erase(net);

	std::set<global_identity> neighbors_on_active_networks = in_neighbors(mnet, identity, active_networks);
	std::set<global_identity> neighbors_on_other_networks = in_neighbors(mnet, identity, other_networks);

	for (global_identity neighbor: neighbors_on_other_networks)
		neighbors_on_active_networks.erase(neighbor);
	return neighbors_on_active_networks;
}

std::set<std::string> in_xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	std::set<std::string> other_networks = mnet.getNetworkNames();
	for (std::string net: active_networks)
		other_networks.erase(net);

	std::set<std::string> neighbors_on_active_networks = in_neighbors(mnet, global_name, active_networks);
	std::set<std::string> neighbors_on_other_networks = in_neighbors(mnet, global_name, other_networks);

	for (std::string neighbor: neighbors_on_other_networks)
		neighbors_on_active_networks.erase(neighbor);
	return neighbors_on_active_networks;
}

std::set<global_identity> in_xneighbors(const MultiplexNetwork& mnet, global_identity identity, network_id network) {
	std::set<network_id> active_networks;
	active_networks.insert(network);
	return in_xneighbors(mnet, identity, active_networks);
}

std::set<std::string> in_xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name) {
	std::set<std::string> active_networks;
	active_networks.insert(network_name);
	return in_xneighbors(mnet, global_name, active_networks);
}

std::set<global_identity> xneighbors(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks) {
	std::set<network_id> other_networks = mnet.getNetworks();
	for (network_id net: active_networks)
		other_networks.erase(net);

	std::set<global_identity> neighbors_on_active_networks = neighbors(mnet, identity, active_networks);
	std::set<global_identity> neighbors_on_other_networks = neighbors(mnet, identity, other_networks);

	for (global_identity neighbor: neighbors_on_other_networks)
		neighbors_on_active_networks.erase(neighbor);
	return neighbors_on_active_networks;
}

std::set<std::string> xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	std::set<std::string> other_networks = mnet.getNetworkNames();
	for (std::string net: active_networks)
		other_networks.erase(net);

	std::set<std::string> neighbors_on_active_networks = neighbors(mnet, global_name, active_networks);
	std::set<std::string> neighbors_on_other_networks = neighbors(mnet, global_name, other_networks);

	for (std::string neighbor: neighbors_on_other_networks)
		neighbors_on_active_networks.erase(neighbor);
	return neighbors_on_active_networks;
}

std::set<global_identity> xneighbors(const MultiplexNetwork& mnet, global_identity identity, network_id network) {
	std::set<network_id> active_networks;
	active_networks.insert(network);
	return xneighbors(mnet, identity, active_networks);
}

std::set<std::string> xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name) {
	std::set<std::string> active_networks;
	active_networks.insert(network_name);
	return xneighbors(mnet, global_name, active_networks);
}
