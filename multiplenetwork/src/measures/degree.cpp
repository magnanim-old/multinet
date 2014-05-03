/*
 * degree.cpp
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "measures.h"

long out_degree(MultilayerNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks) {
	int tmp_degree = 0;
	for (std::set<network_id>::iterator net=active_networks.begin(); net!=active_networks.end(); ++net) {
		tmp_degree += mnet.getNetwork(*net)->getOutDegree(mnet.getLocalVertexId(vertex,*net));
	}
	return tmp_degree;

}

long out_degree(MultilayerNetwork& mnet, std::string vertex, std::set<std::string> active_networks) {
	int tmp_degree = 0;
	for (std::set<std::string>::iterator net=active_networks.begin(); net!=active_networks.end(); ++net) {
		tmp_degree += mnet.getNetwork(*net)->getOutDegree(mnet.getLocalVertexName(vertex,*net));
	}
	return tmp_degree;
}

long out_degree(MultilayerNetwork& mnet, vertex_id vertex, network_id network) {
	return mnet.getNetwork(network)->getOutDegree(mnet.getLocalVertexId(vertex,network));
}

long out_degree(MultilayerNetwork& mnet, std::string vertex_name, std::string network_name) {
	return mnet.getNetwork(network_name)->getOutDegree(mnet.getLocalVertexName(vertex_name,network_name));
}

long in_degree(MultilayerNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks) {
	int tmp_degree = 0;
	for (std::set<network_id>::iterator net=active_networks.begin(); net!=active_networks.end(); ++net) {
		tmp_degree += mnet.getNetwork(*net)->getInDegree(mnet.getLocalVertexId(vertex,*net));
	}
	return tmp_degree;

}

long in_degree(MultilayerNetwork& mnet, std::string vertex, std::set<std::string> active_networks) {
	int tmp_degree = 0;
	for (std::set<std::string>::iterator net=active_networks.begin(); net!=active_networks.end(); ++net) {
		tmp_degree += mnet.getNetwork(*net)->getInDegree(mnet.getLocalVertexName(vertex,*net));
	}
	return tmp_degree;

}

long in_degree(MultilayerNetwork& mnet, vertex_id vertex, network_id network) {
	return mnet.getNetwork(network)->getInDegree(mnet.getLocalVertexId(vertex,network));
}

long in_degree(MultilayerNetwork& mnet, std::string vertex, std::string network_name) {
	return mnet.getNetwork(network_name)->getInDegree(mnet.getLocalVertexName(vertex,network_name));
}

long degree(MultilayerNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks) {
	int tmp_degree = 0;
	for (std::set<network_id>::iterator net=active_networks.begin(); net!=active_networks.end(); ++net) {
		tmp_degree += mnet.getNetwork(*net)->getDegree(mnet.getLocalVertexId(vertex,*net));
	}
	return tmp_degree;
}

long degree(MultilayerNetwork& mnet, vertex_id vertex, network_id network) {
	return mnet.getNetwork(network)->getDegree(mnet.getLocalVertexId(vertex,network));
}

long degree(MultilayerNetwork& mnet, std::string vertex, std::set<std::string> active_networks) {
	int tmp_degree = 0;
	for (std::set<std::string>::iterator net=active_networks.begin(); net!=active_networks.end(); ++net) {
		tmp_degree += mnet.getNetwork(*net)->getDegree(mnet.getLocalVertexName(vertex,*net));
	}
	return tmp_degree;
}

long degree(MultilayerNetwork& mnet, std::string vertex, std::string network_name) {
	return mnet.getNetwork(network_name)->getDegree(mnet.getLocalVertexName(vertex,network_name));
}
