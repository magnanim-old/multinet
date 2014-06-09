/*
 * degree.cpp
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "measures.h"

long out_degree(const MultiplexNetwork& mnet, global_identity global_id, const std::set<network_id>& active_networks) {
	int tmp_degree = 0;
	for (network_id net: active_networks) {
		if (!mnet.containsVertex(global_id,net)) continue;
		tmp_degree += mnet.getNetwork(net).getOutDegree(mnet.getVertexId(global_id,net));
	}
	return tmp_degree;
}

long out_degree(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	int tmp_degree = 0;
	for (std::string network_name: active_networks) {
		if (!mnet.containsVertex(global_name,network_name)) continue;
		tmp_degree += mnet.getNetwork(network_name).getOutDegree(mnet.getVertexName(global_name,network_name));
	}
	return tmp_degree;
}

long out_degree(const MultiplexNetwork& mnet, global_identity global_id, network_id network) {
	if (!mnet.containsVertex(global_id,network)) return 0;
	return mnet.getNetwork(network).getOutDegree(mnet.getVertexId(global_id,network));
}

long out_degree(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name) {
	if (!mnet.containsVertex(global_name,network_name)) return 0;
	return mnet.getNetwork(network_name).getOutDegree(mnet.getVertexName(global_name,network_name));
}

long in_degree(const MultiplexNetwork& mnet, global_identity global_id, const std::set<network_id>& active_networks) {
	int tmp_degree = 0;
	for (network_id net: active_networks) {
		if (!mnet.containsVertex(global_id,net)) continue;
		tmp_degree += mnet.getNetwork(net).getInDegree(mnet.getVertexId(global_id,net));
	}
	return tmp_degree;
}

long in_degree(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	int tmp_degree = 0;
	for (std::string network_name: active_networks) {
		if (!mnet.containsVertex(global_name,network_name)) continue;
		tmp_degree += mnet.getNetwork(network_name).getInDegree(mnet.getVertexName(global_name,network_name));
	}
	return tmp_degree;
}

long in_degree(const MultiplexNetwork& mnet, global_identity global_id, network_id network) {
	if (!mnet.containsVertex(global_id,network)) return 0;
	return mnet.getNetwork(network).getInDegree(mnet.getVertexId(global_id,network));
}

long in_degree(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name) {
	if (!mnet.containsVertex(global_name,network_name)) return 0;
	return mnet.getNetwork(network_name).getInDegree(mnet.getVertexName(global_name,network_name));
}

long degree(const MultiplexNetwork& mnet, global_identity global_id, const std::set<network_id>& active_networks) {
	int tmp_degree = 0;
	for (network_id net: active_networks) {
		if (!mnet.containsVertex(global_id,net)) continue;
		tmp_degree += mnet.getNetwork(net).getDegree(mnet.getVertexId(global_id,net));
	}
	return tmp_degree;
}

long degree(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	int tmp_degree = 0;
	for (std::string network_name: active_networks) {
		if (!mnet.containsVertex(global_name,network_name)) continue;
		tmp_degree += mnet.getNetwork(network_name).getDegree(mnet.getVertexName(global_name,network_name));
	}
	return tmp_degree;
}

long degree(const MultiplexNetwork& mnet, global_identity global_id, network_id network) {
	if (!mnet.containsVertex(global_id,network)) return 0;
	return mnet.getNetwork(network).getDegree(mnet.getVertexId(global_id,network));
}

long degree(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name) {
	if (!mnet.containsVertex(global_name,network_name)) return 0;
	return mnet.getNetwork(network_name).getDegree(mnet.getVertexName(global_name,network_name));
}
