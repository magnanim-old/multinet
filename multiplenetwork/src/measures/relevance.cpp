/*
 * degree.cpp
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "measures.h"
#include <iostream>

double relevance(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks) {
	std::set<network_id> all_networks = mnet.getNetworks();

	double num_neighbors_on_active_networks = neighbors(mnet, identity, active_networks).size();
	double num_neighbors_on_all_networks = neighbors(mnet, identity, all_networks).size();

	if (num_neighbors_on_all_networks==0) return 0; // by definition

	return num_neighbors_on_active_networks/num_neighbors_on_all_networks;
}

double relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	std::set<std::string> all_networks = mnet.getNetworkNames();

	double num_neighbors_on_active_networks = neighbors(mnet, global_name, active_networks).size();
	double num_neighbors_on_all_networks = neighbors(mnet, global_name, all_networks).size();

	if (num_neighbors_on_all_networks==0) return 0; // by definition

	return num_neighbors_on_active_networks/num_neighbors_on_all_networks;
}

double relevance(const MultiplexNetwork& mnet, global_identity identity, network_id network) {
	std::set<network_id> active_networks;
	active_networks.insert(network);
	return relevance(mnet, identity, active_networks);
}

double relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name) {
	std::set<std::string> active_networks;
	active_networks.insert(network_name);
	return relevance(mnet, global_name, active_networks);
}

double in_relevance(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks) {
	std::set<network_id> all_networks = mnet.getNetworks();

	double num_neighbors_on_active_networks = in_neighbors(mnet, identity, active_networks).size();
	double num_neighbors_on_all_networks = in_neighbors(mnet, identity, all_networks).size();

	if (num_neighbors_on_all_networks==0) return 0; // by definition

	return num_neighbors_on_active_networks/num_neighbors_on_all_networks;
}

double in_relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	std::set<std::string> all_networks = mnet.getNetworkNames();

	double num_neighbors_on_active_networks = in_neighbors(mnet, global_name, active_networks).size();
	double num_neighbors_on_all_networks = in_neighbors(mnet, global_name, all_networks).size();

	if (num_neighbors_on_all_networks==0) return 0; // by definition

	return num_neighbors_on_active_networks/num_neighbors_on_all_networks;
}

double in_relevance(const MultiplexNetwork& mnet, global_identity identity, network_id network) {
	std::set<network_id> active_networks;
	active_networks.insert(network);
	return in_relevance(mnet, identity, active_networks);
}

double in_relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name) {
	std::set<std::string> active_networks;
	active_networks.insert(network_name);
	return in_relevance(mnet, global_name, active_networks);
}

double out_relevance(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks) {
	std::set<network_id> all_networks = mnet.getNetworks();

	double num_neighbors_on_active_networks = out_neighbors(mnet, identity, active_networks).size();
	double num_neighbors_on_all_networks = out_neighbors(mnet, identity, all_networks).size();

	if (num_neighbors_on_all_networks==0) return 0; // by definition

	return num_neighbors_on_active_networks/num_neighbors_on_all_networks;
}

double out_relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	std::set<std::string> all_networks = mnet.getNetworkNames();

	double num_neighbors_on_active_networks = out_neighbors(mnet, global_name, active_networks).size();
	double num_neighbors_on_all_networks = out_neighbors(mnet, global_name, all_networks).size();

	if (num_neighbors_on_all_networks==0) return 0; // by definition

	return num_neighbors_on_active_networks/num_neighbors_on_all_networks;
}

double out_relevance(const MultiplexNetwork& mnet, global_identity identity, network_id network) {
	std::set<network_id> active_networks;
	active_networks.insert(network);
	return out_relevance(mnet, identity, active_networks);
}

double out_relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name) {
	std::set<std::string> active_networks;
	active_networks.insert(network_name);
	return out_relevance(mnet, global_name, active_networks);
}

// EXCLUSIVE NETWORK RELEVANCE


double xrelevance(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks) {
	std::set<network_id> all_networks = mnet.getNetworks();

	double num_neighbors_only_on_active_networks = xneighbors(mnet, identity, active_networks).size();
	double num_neighbors_on_all_networks = neighbors(mnet, identity, all_networks).size();

	if (num_neighbors_on_all_networks==0) return 0; // by definition

	return num_neighbors_only_on_active_networks/num_neighbors_on_all_networks;
}

double xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	std::set<std::string> all_networks = mnet.getNetworkNames();

	double num_neighbors_only_on_active_networks = xneighbors(mnet, global_name, active_networks).size();
	double num_neighbors_on_all_networks = neighbors(mnet, global_name, all_networks).size();

	if (num_neighbors_on_all_networks==0) return 0; // by definition

	return num_neighbors_only_on_active_networks/num_neighbors_on_all_networks;
}

double xrelevance(const MultiplexNetwork& mnet, global_identity identity, network_id network) {
	std::set<network_id> active_networks;
	active_networks.insert(network);
	return xrelevance(mnet, identity, active_networks);
}

double xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name) {
	std::set<std::string> active_networks;
	active_networks.insert(network_name);
	return xrelevance(mnet, global_name, active_networks);
}

double in_xrelevance(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks) {
	std::set<network_id> all_networks = mnet.getNetworks();

	double num_neighbors_only_on_active_networks = in_xneighbors(mnet, identity, active_networks).size();
	double num_neighbors_on_all_networks = in_neighbors(mnet, identity, all_networks).size();

	if (num_neighbors_on_all_networks==0) return 0; // by definition

	return num_neighbors_only_on_active_networks/num_neighbors_on_all_networks;
}

double in_xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	std::set<std::string> all_networks = mnet.getNetworkNames();

	double num_neighbors_only_on_active_networks = in_xneighbors(mnet, global_name, active_networks).size();
	double num_neighbors_on_all_networks = in_neighbors(mnet, global_name, all_networks).size();

	if (num_neighbors_on_all_networks==0) return 0; // by definition

	return num_neighbors_only_on_active_networks/num_neighbors_on_all_networks;
}

double in_xrelevance(const MultiplexNetwork& mnet, global_identity identity, network_id network) {
	std::set<network_id> active_networks;
	active_networks.insert(network);
	return in_xrelevance(mnet, identity, active_networks);
}

double in_xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name) {
	std::set<std::string> active_networks;
	active_networks.insert(network_name);
	return in_xrelevance(mnet, global_name, active_networks);
}

double out_xrelevance(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks) {
	std::set<network_id> all_networks = mnet.getNetworks();

	double num_neighbors_only_on_active_networks = out_xneighbors(mnet, identity, active_networks).size();
	double num_neighbors_on_all_networks = out_neighbors(mnet, identity, all_networks).size();

	if (num_neighbors_on_all_networks==0) return 0; // by definition

	return num_neighbors_only_on_active_networks/num_neighbors_on_all_networks;
}

double out_xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks) {
	std::set<std::string> all_networks = mnet.getNetworkNames();

	double num_neighbors_only_on_active_networks = out_xneighbors(mnet, global_name, active_networks).size();
	double num_neighbors_on_all_networks = out_neighbors(mnet, global_name, all_networks).size();

	if (num_neighbors_on_all_networks==0) return 0; // by definition

	return num_neighbors_only_on_active_networks/num_neighbors_on_all_networks;
}

double out_xrelevance(const MultiplexNetwork& mnet, global_identity identity, network_id network) {
	std::set<network_id> active_networks;
	active_networks.insert(network);
	return out_xrelevance(mnet, identity, active_networks);
}

double out_xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name) {
	std::set<std::string> active_networks;
	active_networks.insert(network_name);
	return out_xrelevance(mnet, global_name, active_networks);
}

