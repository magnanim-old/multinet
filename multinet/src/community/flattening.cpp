#include "utils.h"
#include "community.h"
#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "community/flattening.h"
#include "datastructures.h"

namespace mlnet {

/**
 * @brief calculate the weigh of an edge in case the weightening type "NumOfLayers" is chosen.
 * The weight for an edge in the flattened layer  will be set to the number of layers
 * in which the two corresponding actors are connected.
 * @mnet : the multi-layer network instance
 * @act1 : the first actor
 * @act2 : the second actor
 * @return : a double value representing the corresponding weight
 **/
double numOfDimentionsWeightening(const MLNetworkSharedPtr& mnet,const ActorSharedPtr& act1, const ActorSharedPtr& act2){

		/*Inialization*/
		double expectedWeight =0;
		/*for each layer in the input multi-layer network try to find the corresponding nodes of input actors*/
		for(LayerSharedPtr layer: *mnet->get_layers())
		{
			NodeSharedPtr node1 = mnet->get_node(act1,layer);
			NodeSharedPtr node2 = mnet->get_node(act2,layer);

			/*if the corresponding nodes were found, and there is an edge among them, then increase the weight*/
				if(node1 && node2 && mnet->get_edge(node1,node2))
				{
					expectedWeight+=1;
				}
		}
		return expectedWeight/(double) mnet->get_layers()->size();
}


/**
 * @brief calculate the weigh of an edge in case the weightening type "Neighborhood" is chosen.
 * The weight for an edge in the flattened layer  will be set to the ratio constituted by the number
 * of common neighbours of the two actors as a numerator and the total number of neighbours as a denumerator
 * @mnet : the multi-layer network instance
 * @act1 : the first actor
 * @act2 : the second actor
 * @return : a double value representing the corresponding weight
 **/
double neighborhoodWeightening(const MLNetworkSharedPtr& mnet,const ActorSharedPtr& act1, const ActorSharedPtr& act2){
		/*initialization*/
		double expectedWeight =0;
		int numerator=0;
		int denumerator =0;
		int numOfCommonNeighbors =0;
		int numOfNeighborsNode1=0;
		int numOfNeighborsNode2=0;
		/*find the neighbours of these two actors in all every layer*/
		for(LayerSharedPtr layer: *mnet->get_layers())
		{
			/*skip the flattened layer that we added*/
			if(layer->name !="flattened" ){
			/*first find the corresponding nodes of these actors in this layer*/
			NodeSharedPtr node1 = mnet->get_node(act1,layer);
			NodeSharedPtr node2 = mnet->get_node(act2,layer);
			/*if they have corresponding nodes in this layer*/
			if(node1 && node2){
			/*we need to count the common neighbours and the total number of neighbours*/
			numOfCommonNeighbors =0;
			numOfNeighborsNode1=0;
			numOfNeighborsNode2=0;
			/*get the neighbours of each node*/
			NodeListSharedPtr node1Neighbours = mnet->neighbors(node1,INOUT);
			NodeListSharedPtr node2Neighbours = mnet->neighbors(node2,INOUT);
			/*if they both have neighbours, then continue*/
			if(node1Neighbours && node2Neighbours){
			/*for each neighbour of node 1*/
			for (NodeSharedPtr neighborOfN1 : *node1Neighbours) {
				/*first check that it is an intra-layer neighbour*/
				if(neighborOfN1->layer==layer){
				/*if yes, then count him*/
				numOfNeighborsNode1++;
				numOfNeighborsNode2=0;
				/*for each neighbour of node 2*/
				for (NodeSharedPtr neighborOfN2 : *mnet->neighbors(node2,INOUT)) {
				/*check that it is an intra-layer neighbour */
				if(neighborOfN2->layer==layer){
				/*if yes, then count him*/
				numOfNeighborsNode2++;
				/*if it is a common neighbour, then increase*/
				if(neighborOfN1==neighborOfN2){
					numOfCommonNeighbors++;}}}
			  }
			 }
		   }
		  }
		  numerator+=numOfCommonNeighbors;
		  denumerator = denumerator+ numOfNeighborsNode1+numOfNeighborsNode2;
		 }
		}
		if(denumerator!=0){
		expectedWeight= (double)numerator/(double)denumerator;
		}
		return expectedWeight;

 }


/**
 * @brief calculate the weigh of an edge in case the weightening type "Jaccard" is chosen.
 * The weight for an edge in the flattened layer  will be set to the ratio constituted by the number
 * of common layers of the two actors as a numerator and the union of layers as a denumerator
 * @mnet : the multi-layer network instance
 * @act1 : the first actor
 * @act2 : the second actor
 * @return : a double value representing the corresponding weight
 **/
double jaccardWeghtening(const MLNetworkSharedPtr& mnet,const ActorSharedPtr& act1, const ActorSharedPtr& act2){


	std::unordered_set<std::string> a1_layers ;
	std::unordered_set<std::string> a2_layers ;

    for (LayerSharedPtr layer: *mnet->get_layers()) {
   	   if (mnet->get_node(act1,layer)) {
   		   a1_layers.insert(layer->name);
   	   }
   	   if (mnet->get_node(act2,layer)) {
   	   	   a2_layers.insert(layer->name);
   	   }
    }

    std::vector<std::unordered_set<std::string> > arr;
    arr.push_back(a1_layers);
    arr.push_back(a2_layers);


	return jaccard_similarity(arr);

}

/**
 * @brief Flatten a multiplex network into one single layer weighted network.
 * @mnet:  The input multiplex network instance
 * @wType: The weightening method to be implemented in the resulted edges in the flattened graph
 * @return:The result of weighted flattening (i.e : a single layer weighted network)
 **/



MLNetworkSharedPtr  flatten(const MLNetworkSharedPtr& mnet, WeighteningType wType){

 //STEPS:
	MLNetworkSharedPtr fnet = MLNetwork::create("flattened_net");

 //(1) Mapping function : from multi-dimentional(multiplex) to mono-dimentional(flattened layer)

	/*add a new layer to the flattened network*/
	LayerSharedPtr flattenedLayer =  fnet->add_layer("flattened",UNDIRECTED);

	/*add the weight attribute to the edges of this layer*/
	fnet->edge_features(flattenedLayer,flattenedLayer)->add("_WEIGHT",NUMERIC_TYPE);

	/*add the actors to the flattened network */
	for (ActorSharedPtr act:*mnet->get_actors()){
		fnet->add_actor(act->name);
		fnet->add_node(act,flattenedLayer);
	}

	/*for each edge in the input network*/
	for (EdgeSharedPtr edge : *mnet->get_edges()) {
		/*consider the edge only if is an intra-layer edge*/
		if(edge->v1->layer == edge->v2->layer)
		{
			ActorSharedPtr act1 = mnet->get_actor(edge->v1->actor->name);
			ActorSharedPtr act2 = mnet->get_actor(edge->v2->actor->name);

			NodeSharedPtr from = fnet->get_node(act1,flattenedLayer);
			NodeSharedPtr to = fnet->get_node(act2,flattenedLayer);
			double expectedWeight=0;
			//Add the edge if the it is not already existent
			if(!fnet->get_edge(from,to))
			{
				fnet->add_edge(from,to);

				//calculate the weight of the edge
				switch (wType) {
						case ZeroOne:
							expectedWeight =1;
							break;
						case NumOfLayers:
							expectedWeight = numOfDimentionsWeightening(mnet,act1,act2) ;
							break;
						case Neighborhood:
							expectedWeight = neighborhoodWeightening(mnet,act1,act2);
							 break;
						case Jaccard:
							expectedWeight = jaccardWeghtening(mnet,act1,act2);
							break;
						default: expectedWeight =1;
				}
				fnet->set_weight(from,to,expectedWeight);
			}

		}
	}
   return fnet;
	}


/**
 * @brief maps back the communities found in the flattened network into multi-layer communities in the original multi-layer instance.
 * @fComs: The communities of the flattened graph
 * @mnet:  The input multi-layer network instance
 * @return:The mapping as actor communities on the multi-layer instance
 **/
 CommunityStructureSharedPtr map_back_to_ml(const CommunityStructureSharedPtr& fComs,const MLNetworkSharedPtr& mnet){

	ActorCommunityStructureSharedPtr result = actor_community_structure::create();

	//for each community in the flattened network
	for(CommunitySharedPtr singleCom:fComs->get_communities()){
		ActorCommunitySharedPtr actor_com = actor_community::create();

		//for each node in the current flattened network community
		for(NodeSharedPtr node:singleCom->get_nodes()){
			//find the actor of this node in the mnet instance
			ActorSharedPtr actor = node->actor;
			//add this actor to the actor_community
			actor_com->add_actor(actor);
			//get the nodes of this actor in all the layers in the mnet instance
			NodeListSharedPtr  actorNodes =  mnet->get_nodes(actor);
			//add the layers where these nodes were found to the communitiy list of layers
			for(NodeSharedPtr node:*actorNodes){
				if(node->layer->name!="flattened" & actor_com->get_layers().find(node->layer)==actor_com->get_layers().end()){
					actor_com->add_layer(node->layer);
				}
			}
		}
		// add this community to the list of communities to be returned
		result->add_community(actor_com);
	}

	return to_node_communities(result,mnet);
   }

}
