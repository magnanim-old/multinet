/*
 * degree.cpp
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "datastructures.h"
#include "counter.h"
#include <unordered_set>


namespace mlnet {

double jaccard_similarity(const MLNetworkSharedPtr& mnet, const std::unordered_set<LayerSharedPtr>& layers) {
	PairCounter<actor_id,actor_id> c;
	int num_layers = layers.size();
	long num_pairs = 0;
	long num_matches = 0;
	// for each of the edges in this layer, check which other layers contain it
	for (LayerSharedPtr layer: layers) {
		for (EdgeSharedPtr edge: mnet->get_edges(layer, layer)) {
			c.inc(edge->v1->actor->id,edge->v2->actor->id);
			// TODO check self edges
			if (!edge->directed) {
				c.inc(edge->v2->actor->id,edge->v1->actor->id);
			}
		}
	}
	for (auto& e1: c.map()) {
		for (auto& e2: e1.second) {
			num_pairs++;
			if (e2.second==num_layers)
				num_matches++;
		}
	}
	return double(num_matches)/num_pairs;


	/* verify if all networks are of the same kind
	bool directed = mnet.getNetwork(*active_networks.begin()).isDirected();
	std::set<network_id>::const_iterator it;
	for (it=active_networks.begin()+1; it!=active_networks.end(); ++it) {
		if (mnet.getNetwork(*it).isDirected() != directed)
			throw OperationNotSupportedException("cannot compare directed networks with undirected ones");
	}
	Network flat = flatten_weighted(mnet, active_networks,false,false);
	std::set<edge_id> edges = flat.getEdges();
	//std::cout << edges.size() << " " << flat.getNumEdges() << std::endl;
	long num_edges_on_all_networks = 0;
	for (edge_id e: edges) {
		if (flat.getEdgeWeight(e.v1,e.v2)==active_networks.size())
			num_edges_on_all_networks++;
	}
	//std::cout << num_edges_on_all_networks << " " << edges.size() << std::endl;
	return double(num_edges_on_all_networks)/mnet->getedges.size();

	*/
}

double jaccard_similarity(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	std::unordered_set<LayerSharedPtr> layers;
	layers.insert(layer1);
	layers.insert(layer2);
	return jaccard_similarity(mnet,layers);
}

double coverage(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	//std::cout << edges.size() << " " << flat.getNumEdges() << std::endl;
	long num_edges = mnet->get_edges(layer2,layer2).size();
	if (!mnet->is_directed(layer2,layer2))
		num_edges *= 2;
	long num_covered_edges = 0;
	for (EdgeSharedPtr edge: mnet->get_edges(layer1,layer1)) {
		NodeSharedPtr node1 = mnet->get_node(edge->v1->actor,layer2);
		NodeSharedPtr node2 = mnet->get_node(edge->v2->actor,layer2);
		if (!node1 || !node2) // actor not present in layer 2
			continue;
		if (mnet->get_edge(node1,node2))
			num_covered_edges++;
		if (!edge->directed) {
			if (mnet->get_edge(node2,node1))
				num_covered_edges++;
		}
	}
	//std::cout << num_edges_on_all_networks << " " << edges.size() << std::endl;
	return double(num_covered_edges)/num_edges;
}

double assortativity(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2, edge_mode mode) {
	//std::cout << edges.size() << " " << flat.getNumEdges() << std::endl;
	long num_actors_not_in_layer1 = 0;
	long num_actors_not_in_layer2 = 0;
	double mean_degree_layer1 = 0;
	double mean_degree_layer2 = 0;

	// Calculating degrees
	for (NodeSharedPtr node1: mnet->get_nodes(layer1)) {
		mean_degree_layer1 += mnet->neighbors(node1,mode).size();
		NodeSharedPtr node2;
		if ((node2 = mnet->get_node(node1->actor,layer2))) {
			mean_degree_layer2 += mnet->neighbors(node2,mode).size();
		}
		else {
			num_actors_not_in_layer2++;
		}
	}
	// now processing the actors only in layer 2
	for (NodeSharedPtr node2: mnet->get_nodes(layer2)) {
		if (mnet->get_node(node2->actor,layer1)) {
			// already computed in the previous for loop
		}
		else {
			mean_degree_layer2 += mnet->neighbors(node2,mode).size();
			num_actors_not_in_layer1++;
		}
	}
	long num_actors = mnet->get_nodes(layer1).size() + num_actors_not_in_layer1;
	mean_degree_layer1 /= num_actors;
	mean_degree_layer2 /= num_actors;

	// Calculating covariance and standard deviations
	double covariance = 0;
	double stdev1 = 0;
	double stdev2 = 0;
	for (NodeSharedPtr node1: mnet->get_nodes(layer1)) {
		long degree1 = mnet->neighbors(node1,mode).size();
		long degree2 = 0;
		NodeSharedPtr node2;
		if ((node2 = mnet->get_node(node1->actor,layer2))) {
			degree2 = mnet->neighbors(node2,mode).size();
		}
		covariance += (degree1 - mean_degree_layer1)*(degree2 - mean_degree_layer2);
		stdev1 += (degree1 - mean_degree_layer1)*(degree1 - mean_degree_layer1);
		stdev2 += (degree2 - mean_degree_layer2)*(degree2 - mean_degree_layer2);
	}
	// now processing the actors only in layer 2
	for (NodeSharedPtr node2: mnet->get_nodes(layer2)) {
		if (mnet->get_node(node2->actor,layer1)) {
			// already considered in the previous loop
		}
		else {
			long degree2 = mnet->neighbors(node2,mode).size();
			stdev1 += (0 - mean_degree_layer1)*(0 - mean_degree_layer1);
			stdev2 += (degree2 - mean_degree_layer2)*(degree2 - mean_degree_layer2);
			covariance += (0 - mean_degree_layer1)*(degree2 - mean_degree_layer2);
		}
	}
	covariance /= num_actors;
	stdev1 /= num_actors;
	stdev1 = std::sqrt(stdev1);
	stdev2 /= num_actors;
	stdev2 = std::sqrt(stdev2);

	if (stdev1==0 && stdev2==0)
		return 1;
	else if (stdev1==0 || stdev2==0)
		return 0;
	else return covariance/stdev1/stdev2;
}

/*
double network_coverage(const MLNetwork& mnet, const std::set<std::string>& n1, const std::set<std::string>& n2) {
	std::set<network_id> nets1, nets2;
	for (std::string name: n1) {
		nets1.insert(mnet.getNetworkId(name));
	}
	for (std::string name: n2) {
		nets2.insert(mnet.getNetworkId(name));
	}
	return network_coverage(mnet,nets1,nets2);
}

double network_coverage(const MLNetwork& mnet, network_id network1, network_id network2) {
	std::set<network_id> n1, n2;
	n1.insert(network1);
	n2.insert(network2);
	return network_coverage(mnet,n1,n2);
}

double network_coverage(const MLNetwork& mnet, const std::string& network_name1, const std::string& network_name2) {
	std::set<std::string> n1, n2;
	n1.insert(network_name1);
	n2.insert(network_name2);
	return network_coverage(mnet,n1,n2);
}*/

}
