/*
 * degree.cpp
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "measures.h"

long out_degree(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks) {
	int tmp_degree = 0;
	for (std::set<network_id>::iterator net=active_networks.begin(); net!=active_networks.end(); ++net) {
		tmp_degree += mnet.getNetwork(*net)->getOutDegree(vertex);
	}
	return tmp_degree;

}

long out_degree(MultipleNetwork& mnet, std::string vertex, std::set<std::string> active_networks) {
	int tmp_degree = 0;
	for (std::set<std::string>::iterator net=active_networks.begin(); net!=active_networks.end(); ++net) {
		tmp_degree += mnet.getNetwork(*net)->getOutDegree(vertex);
	}
	return tmp_degree;
}

long out_degree(MultipleNetwork& mnet, vertex_id vertex, network_id network) {
	return mnet.getNetwork(network)->getOutDegree(vertex);
}

long out_degree(MultipleNetwork& mnet, std::string vertex_name, std::string network_name) {
	return mnet.getNetwork(network_name)->getOutDegree(vertex_name);
}

long in_degree(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks) {
	int tmp_degree = 0;
	for (std::set<network_id>::iterator net=active_networks.begin(); net!=active_networks.end(); ++net) {
		tmp_degree += mnet.getNetwork(*net)->getInDegree(vertex);
	}
	return tmp_degree;

}

long in_degree(MultipleNetwork& mnet, std::string vertex, std::set<std::string> active_networks) {
	int tmp_degree = 0;
	for (std::set<std::string>::iterator net=active_networks.begin(); net!=active_networks.end(); ++net) {
		tmp_degree += mnet.getNetwork(*net)->getInDegree(vertex);
	}
	return tmp_degree;

}

long in_degree(MultipleNetwork& mnet, vertex_id vertex, network_id network) {
	return mnet.getNetwork(network)->getInDegree(vertex);
}

long in_degree(MultipleNetwork& mnet, std::string vertex, std::string network_name) {
	return mnet.getNetwork(network_name)->getInDegree(vertex);
}

long degree(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks) {
	int tmp_degree = 0;
	for (std::set<network_id>::iterator net=active_networks.begin(); net!=active_networks.end(); ++net) {
		tmp_degree += mnet.getNetwork(*net)->getDegree(vertex);
	}
	return tmp_degree;
}

long degree(MultipleNetwork& mnet, vertex_id vertex, network_id network) {
	return mnet.getNetwork(network)->getDegree(vertex);
}

long degree(MultipleNetwork& mnet, std::string vertex, std::set<std::string> active_networks) {
	int tmp_degree = 0;
	for (std::set<std::string>::iterator net=active_networks.begin(); net!=active_networks.end(); ++net) {
		tmp_degree += mnet.getNetwork(*net)->getDegree(vertex);
	}
	return tmp_degree;
}

long degree(MultipleNetwork& mnet, std::string vertex, std::string network_name) {
	return mnet.getNetwork(network_name)->getDegree(vertex);
}
