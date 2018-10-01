/*
 * modularity.cpp
 *
 * Created on: Feb 27, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include <unordered_map>
#include <iostream>


#include "community.h"

namespace mlnet {

/**
* calculates the belonging factors for nodes to their communities by setting it equal to 1 if the node is a non-overlapping node,
* or 1/#communities_for_the_node when the node is an overlapping node
* @param communities: set of communities (groups of nodes that belong to mnet)
*/
hash_map<CommunitySharedPtr,hash_map<NodeSharedPtr,double>> get_nodes_belonging_factors(const CommunityStructureSharedPtr& communities){

	//we define node_frequecy here as the number of communities the node is part of
	//std::cout<< "get_nodes_belonging_factors" << std::endl;
	hash_map<NodeSharedPtr,long> nodes_frequencies;
	vector<CommunitySharedPtr> communities_list = communities->get_communities();
	for (CommunitySharedPtr com:communities_list){
		for(NodeSharedPtr node:com->get_nodes()){
			if(nodes_frequencies.find(node) == nodes_frequencies.end()){
				nodes_frequencies[node]=1;
			}
			else nodes_frequencies[node]++;
		}
	}

	//initialise the output structure
	hash_map<CommunitySharedPtr,hash_map<NodeSharedPtr,double>> nodes_belonging_coef;

	//set equal contributions for the node in each community it is part of
	for (CommunitySharedPtr com:communities_list){
			for(NodeSharedPtr node:com->get_nodes()){
				nodes_belonging_coef[com][node]= (double)1/nodes_frequencies[node];
			}
		}
	//std::cout<< "end get_nodes_belonging_factors" << std::endl;
	return nodes_belonging_coef;
}


/**
* calculates the edge belonging factors to its community as a function of its corresponding nodes belonging factors
* @param node1_belonging_co: first node belonging factor
* @param node2_belonging_co: second node belonging factor
* @param func could be one of the following values (Sum,Multiply,Max,Average).
*/
double F(double node1_belonging_co,double node2_belonging_co, EdgeBelonigngFunc func){
	double belonging =0;
	switch (func) {
		case Sum:
			belonging=node1_belonging_co+node2_belonging_co;
			break;
		case Multiply:
			belonging=node1_belonging_co*node2_belonging_co;
			break;
		case Max:
			belonging= (node1_belonging_co>node2_belonging_co)? node1_belonging_co:node2_belonging_co;
			break;
		case Average:
			belonging=(node1_belonging_co+node2_belonging_co)/2;
			break;
		default:
			belonging=node1_belonging_co*node2_belonging_co;
	}
	return belonging;
}


/**
# Implementation of multislice modularity defined in :
# Mucha, P. J., Richardson, T., Macon, K., Porter, M. A. & Onnela, J.-P.
# Community structure in time - dependent, multiscale, and multiplex networks.
# Science 328, 876–8(2010)
* @param mnet : the multi-layer instance
* @param communities: a clustering
* @param interlayer_weight: 0-1
 */
double modularity(const MLNetworkSharedPtr& mnet, const CommunityStructureSharedPtr& communities, double interlayer_weight){

	double gamma=1.0;

	hash_map<NodeSharedPtr,int> assignments;
	int c_num = 0;
	for (CommunitySharedPtr com: communities->get_communities()) {
		 for (NodeSharedPtr node: com->get_nodes()) {
			 assignments[node]=c_num;
		 }
		 c_num++;
	}
	double mu =0.0;
	double Q = 0.0;
    //For each layer
	for (LayerSharedPtr layer: *mnet->get_layers()) {
		//For each node in that layer
		for (NodeSharedPtr node:*mnet->get_nodes(layer)){
			double sum_i = 0.0;
			//store the degree of this node
			int deg_node = mnet->neighbors(node,INOUT)->size();
			//for each neighbour pf this node
			for (NodeSharedPtr nei: *mnet->neighbors(node,INOUT)){
				//check if they both (the node and its neighbour) are assigned to a community
				if(assignments.find(node)!=assignments.end() && assignments.find(nei) != assignments.end()){
					//check if they both (the node and its neighbour) are in the same community
					if(assignments[node] == assignments[nei]){
					 //store the degree of the neighbour
					 int deg_nei = mnet->neighbors(nei,INOUT)->size();
					 //calculate the contribution of this edge to the modularity
					 sum_i+= 1- gamma*((deg_node*deg_nei)/(2*mnet->get_edges(layer,layer)->size()));
					}
				}
			}
			sum_i+=interlayer_weight;
			Q+=sum_i;
			mu+=deg_node+(mnet->get_layers()->size()-1)*interlayer_weight;
		}
	}

	Q=(double) Q/(2*mu);
	return Q;

}

/**
* calculate the modularity for graphs with overlapping communities where in this implementation, two communities are overlapping if they share nodes
* reference :  http://iopscience.iop.org/article/10.1088/1742-5468/2009/03/P03024/meta
* @param mnet : the multi-layer instance
* @param communities: a clustering
* @param interlayer_weight: 0-1
*/
double extended_modularity(const MLNetworkSharedPtr& mnet, const CommunityStructureSharedPtr& communities, double interlayer_weight){

	double gamma=1.0;

	hash_map<CommunitySharedPtr,hash_map<NodeSharedPtr,double>> n_bc = get_nodes_belonging_factors(communities);//nodes belonging co-effiecinets to communities
	hash_map<NodeSharedPtr,hash_set<CommunitySharedPtr>> assignments;

	for (CommunitySharedPtr com: communities->get_communities()) {
		 for (NodeSharedPtr node: com->get_nodes()) {
			 assignments[node].insert(com);
		 }
	}

	double mu =0.0;
	double Q = 0.0;
	//For each layer
	for (LayerSharedPtr layer: *mnet->get_layers()) {
		//For each node in that layer
		for (NodeSharedPtr node:*mnet->get_nodes(layer)){
			double sum_i = 0.0;
			//Store the degree of this node
			int deg_node = mnet->neighbors(node,INOUT)->size();
			//For each neighbour of this node
			for (NodeSharedPtr nei: *mnet->neighbors(node,INOUT)){
				//If they both are assigned to a community
				if(assignments.find(node)!=assignments.end() && assignments.find(nei) != assignments.end()){
					//Store the intersection of community assignments for both (the node and its neighcour)
					hash_set<CommunitySharedPtr> same_coms = s_intersection(assignments[node] ,assignments[nei]);
					//Get the degree of the neighbour
					int deg_nei = mnet->neighbors(nei,INOUT)->size();
					//For each common community assignment for both (the node and its neighbour)
					for(CommunitySharedPtr shared_com:same_coms){

						long bc_i_c = 0; //expected belonging factor of any link starting from "node" to "shaed_com" (in a null model)
						for (NodeSharedPtr out_node: shared_com->get_nodes()) {
						bc_i_c += F(n_bc[shared_com][node],n_bc[shared_com][out_node],Max)/mnet->get_nodes()->size();
						}

						long bc_j_c = 0; //expected belonging factor of any link ending at "nei" to "shaed_com" (in a null model)
						for (NodeSharedPtr in_node: shared_com->get_nodes()) {
						bc_j_c += F(n_bc[shared_com][in_node],n_bc[shared_com][nei],Max)/mnet->get_nodes()->size();
						}

						long bc_ij_c = F(n_bc[shared_com][node],n_bc[shared_com][nei],Max);// calculates the belonging factor of an edge to a community as a function of the belonging factors of the nodes

						sum_i+= bc_ij_c- gamma*((bc_i_c*deg_node*bc_j_c*deg_nei)/(2*mnet->get_edges(layer,layer)->size()));
					}
				}
			}
			sum_i+=interlayer_weight;
			Q+=sum_i;
			mu+=deg_node+(mnet->get_layers()->size()-1)*interlayer_weight;
		}
	}
	Q=(double) Q/(2*mu);
	return Q;

}

}
