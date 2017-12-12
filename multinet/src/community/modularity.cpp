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

double modularity(const MLNetworkSharedPtr& mnet, const CommunityStructureSharedPtr& communities, double c) {
	
    double res = 0;
	double mu = 0;
	hash_map<LayerSharedPtr,long> m_s;
	for (LayerSharedPtr s: *mnet->get_layers()) {
		double m = mnet->get_edges(s,s)->size();
		if (!mnet->is_directed(s,s))
			m *= 2;
		// FIX TO THE ORIGINAL EQUATION WHEN THERE ARE NO EDGES
		if (m == 0)
			m = 1; // no effect on the formula
		m_s[s] = m;
		mu += m;
	}

		for (CommunitySharedPtr community: communities->get_communities()) {
            for (NodeSharedPtr i: community->get_nodes()) {
				for (NodeSharedPtr j: community->get_nodes()) {
				if (i==j) continue; // not in the original definition - we do this assuming to deal with simple graphs
				//std::cout << i->to_string() << " " << groups.count(i) << std::endl;
				//std::cout << j->to_string() << " " << groups.count(j) << std::endl;

				if (i->layer==j->layer) {
					//std::cout << "Same group!" << std::endl;
					//if (mnet.getNetwork(net)->containsEdge(*v_i,*v_j))
					//	std::cout << "Edge" << std::endl;
					long k_i = mnet->neighbors(i,OUT)->size();
					long k_j = mnet->neighbors(j,IN)->size();
					int a_ij = mnet->get_edge(i,j)? 1.0 : 0.0;
					res += a_ij - (double)k_i * k_j / (m_s.at(i->layer));
					//std::cout << i->actor->name << " " << j->actor->name << " " << i->layer->name << " "<< k_i << " " <<  k_j << " " <<  m_s.at(i->layer) << std::endl;
					//std::cout << "->" << res << std::endl;
				}
				if (i->actor==j->actor) {
					res += c;
				}
			}
		}
		//std::cout << "->" << m_net << std::endl;
	}
	//std::cout << "same" << std::endl;

	//std::cout << mu << std::endl;
	for (ActorSharedPtr actor: *mnet->get_actors()) {
		int num_nodes = mnet->get_nodes(actor)->size();
		mu+=num_nodes*(num_nodes-1)*c; // unclear if we should multiply by c
	}
	//std::cout << mu << std::endl;

	return 1 / mu * res;
}

double modularity(const MLNetworkSharedPtr& mnet, const hash_map<NodeSharedPtr,long>& membership, double c) {
        // partition the nodes by group
        hash_map<long, std::set<NodeSharedPtr> > groups;
        for (auto pair: membership) {
            groups[pair.second].insert(pair.first);
        }
        // start computing the modularity
        double res = 0;
        double mu = 0;
        hash_map<LayerSharedPtr,long> m_s;
        for (LayerSharedPtr s: *mnet->get_layers()) {
            double m = mnet->get_edges(s,s)->size();
            if (!mnet->is_directed(s,s))
                m *= 2;
            // FIX TO THE ORIGINAL EQUATION WHEN THERE ARE NO EDGES
            if (m == 0)
                m = 1; // no effect on the formula
            m_s[s] = m;
            mu += m;
        }
        
        for (auto pair: groups) {
            for (NodeSharedPtr i: pair.second) {
                for (NodeSharedPtr j: pair.second) {
                    if (i==j) continue; // not in the original definition - we do this assuming to deal with simple graphs
                    //std::cout << i->to_string() << " " << groups.count(i) << std::endl;
                    //std::cout << j->to_string() << " " << groups.count(j) << std::endl;
                    
                    if (i->layer==j->layer) {
                        //std::cout << "Same group!" << std::endl;
                        //if (mnet.getNetwork(net)->containsEdge(*v_i,*v_j))
                        //	std::cout << "Edge" << std::endl;
                        long k_i = mnet->neighbors(i,OUT)->size();
                        long k_j = mnet->neighbors(j,IN)->size();
                        int a_ij = mnet->get_edge(i,j)? 1.0 : 0.0;
                        res += a_ij - (double)k_i * k_j / (m_s.at(i->layer));
                        //std::cout << i->actor->name << " " << j->actor->name << " " << i->layer->name << " "<< k_i << " " <<  k_j << " " <<  m_s.at(i->layer) << std::endl;
                        //std::cout << "->" << res << std::endl;
                    }
                    if (i->actor==j->actor) {
                        res += c;
                    }
                }
            }
            //std::cout << "->" << m_net << std::endl;
        }
        //std::cout << "same" << std::endl;
        
        //std::cout << mu << std::endl;
        for (ActorSharedPtr actor: *mnet->get_actors()) {
            int num_nodes = mnet->get_nodes(actor)->size();
            mu+=num_nodes*(num_nodes-1)*c; // unclear if we should multiply by c
        }
        //std::cout << mu << std::endl;
        
        return 1 / mu * res;
    }



/**
* calculates the belonging coefficients for nodes to their communities by setting it equal to 1 if the node is a non-overlapping node,
* or 1/#communities_for_the_node when the node is an overlapping node
* @param communities: set of communities (groups of nodes that belong to mnet)
*/
hash_map<CommunitySharedPtr,hash_map<NodeSharedPtr,double>> get_nodes_belonging_coef(const CommunityStructureSharedPtr& communities){

	//we define node_frequecy here as the number of communities the node is part of
	std::cout<< "get_nodes_belonging_coef" << std::endl;
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
				nodes_belonging_coef[com][node]=1/nodes_frequencies[node];
			}
		}
	std::cout<< "end get_nodes_belonging_coef" << std::endl;
	return nodes_belonging_coef;
}


/**
* calculates the edge belonging co_efficient to its community as a function of its corresponding nodes belonging coefficients
* @param node1_belonging_co: first node belonging coefficient
* @param node2_belonging_co: second node belonging coefficient
* @param func could be one of the following values (Sum,Multiply,Max,Average).
*/
double get_edge_belonging_coefficient(double node1_belonging_co,double node2_belonging_co, EdgeBelonigngFunc func){
	double edge_belonging =0;
	switch (func) {
		case Sum:
			edge_belonging=node1_belonging_co+node2_belonging_co;
			break;
		case Multiply:
			edge_belonging=node1_belonging_co*node2_belonging_co;
			break;
		case Max:
			edge_belonging= (node1_belonging_co>node2_belonging_co)? node1_belonging_co:node2_belonging_co;
			break;
		case Average:
			edge_belonging=(node1_belonging_co+node2_belonging_co)/2;
			break;
		default:
			edge_belonging=node1_belonging_co*node2_belonging_co;
	}
	return edge_belonging;
}



/**
* calculate the modularity for directed graphs with overlapping communities where in this implementation, two communities are overlapping if they share nodes
* reference :  http://iopscience.iop.org/article/10.1088/1742-5468/2009/03/P03024/meta
* @param mnet : the multi-layer instance
* @param communities: set of communities (groups of nodes that belong to mnet)
* @param nodes_belonging_coefficients (weights given for the nodes that are in the overlapping communities and it express how strongly a node belong to each community it is part of)
* @param func could be one of the following values (Sum,Multiply,Max,Average) and it specifies how the edge belonging to a cummunity should be calculated as a function of the belonging coefficients of the corresponding nodes
*/
double extended_modularity(const MLNetworkSharedPtr& mnet,
						   const CommunityStructureSharedPtr& communities,
						   hash_map<CommunitySharedPtr,hash_map<NodeSharedPtr,double>> nodes_belonging_coefficients,
						   EdgeBelonigngFunc func){


		double sum_of_differences =0;
		double modularity =0;

		//some auxiliary variables

		double first_node_belonging_co=1;
		double second_node_belonging_co= 1;
		double out_neighbour_belonging_co =1;
		double in_neighbour_belonging_co =1;

		double sum_out_edge_belonging_co = 0;
		double sum_in_edge_belonging_co = 0;

		double actual_edge_beloning_co =  0;
		double expected_out_edge_belonging_co =0;
		double expected_in_edge_belonging_co =0;

		//build a map for the communities edges
		std::map<CommunitySharedPtr,std::map<NodeSharedPtr,std::vector<NodeSharedPtr>>> communities_edges;
		for(CommunitySharedPtr com:communities->get_communities()){
			for(NodeSharedPtr node1:com->get_nodes()){
				for(NodeSharedPtr node2:com->get_nodes()){
				 if(node1!=node2 & mnet->get_edge(node1,node2)!=NULL)
					 communities_edges[com][node1].push_back(node2);
				}
			}
		}

		//iterate through communities
		for(CommunitySharedPtr com:communities->get_communities()){
			for(NodeSharedPtr node1:com->get_nodes()){
				for(NodeSharedPtr node2:com->get_nodes()){
					//if it is not the same node and there is an edge between the corresponding two actors
					if(node1 != node2 &  mnet->get_edge(node1,node2)!=NULL){
					 //a-calculate the actual belonging co_efficient of the this link (referred to as r(i,j,c) in the paper)
						first_node_belonging_co=1;
						second_node_belonging_co= 1;
					   //if the nodes belonging coefficient is already given in the input variable nodes_belonging_coefficients
					   if(nodes_belonging_coefficients[com].find(node1)!=nodes_belonging_coefficients[com].end())
						   first_node_belonging_co=nodes_belonging_coefficients[com][node1];
					   if(nodes_belonging_coefficients[com].find(node2)!=nodes_belonging_coefficients[com].end())
						   second_node_belonging_co=nodes_belonging_coefficients[com][node2];

					 actual_edge_beloning_co= get_edge_belonging_coefficient(first_node_belonging_co,second_node_belonging_co,func);


					//b-calculate the expected belonging co_efficient of the this link to this community (referred to as s(i,j,c) in the paper)
					  //b.1- expected belonging co_efficient of any link starting from the node 1  (referred to as B_out_(i,j),c)
						NodeListSharedPtr out_neighbours =  mnet->neighbors(node1,OUT);
						sum_out_edge_belonging_co = 0;
						expected_out_edge_belonging_co =0;
						//if the node has out_neighbours
						if(out_neighbours->size()!=0){
							for(vector<NodeSharedPtr>::iterator nbr_iter= communities_edges[com][node1].begin();nbr_iter!= communities_edges[com][node1].end();++nbr_iter){
								if(nodes_belonging_coefficients[com].find(*nbr_iter)!=nodes_belonging_coefficients[com].end())
									out_neighbour_belonging_co=nodes_belonging_coefficients[com][*nbr_iter];
								else out_neighbour_belonging_co=1;
								sum_out_edge_belonging_co+=get_edge_belonging_coefficient(first_node_belonging_co,out_neighbour_belonging_co,func);;
							}
							expected_out_edge_belonging_co=sum_out_edge_belonging_co/out_neighbours->size();
						}
						else expected_out_edge_belonging_co=0;

						//b.2- expected belonging co_efficient of any link ending at node 2  (referred to as B_in_(i,j),c)
						NodeListSharedPtr in_neighbours =  mnet->neighbors(node2,IN);
						sum_in_edge_belonging_co = 0;
						expected_in_edge_belonging_co =0;
						//if the node has in_neighbours
						if(in_neighbours->size()!=0){
							for(vector<NodeSharedPtr>::iterator nbr_iter= communities_edges[com][node2].begin();nbr_iter!= communities_edges[com][node2].end();++nbr_iter){
								if(nodes_belonging_coefficients[com].find(*nbr_iter)!=nodes_belonging_coefficients[com].end())
									in_neighbour_belonging_co=nodes_belonging_coefficients[com][*nbr_iter];
								else in_neighbour_belonging_co=1;
								sum_in_edge_belonging_co+=get_edge_belonging_coefficient(in_neighbour_belonging_co,second_node_belonging_co,func);;
							}
							expected_in_edge_belonging_co=sum_in_edge_belonging_co/out_neighbours->size();
						}
						else expected_in_edge_belonging_co=0;

					//c-add the difference between expected and actual to the sum of differences
						sum_of_differences+= actual_edge_beloning_co - (expected_out_edge_belonging_co*out_neighbours->size())*(expected_in_edge_belonging_co*in_neighbours->size())/mnet->get_edges()->size();
					}
				}
			}
		}
		modularity=sum_of_differences/mnet->get_edges()->size();
	return modularity;
}

}
