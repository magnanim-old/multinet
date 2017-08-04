
#include "utils.h"
#include "community.h"
#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <fstream>

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
			if(layer->name!="flattened")
			{
			NodeSharedPtr node1 = mnet->get_node(act1,layer);

			NodeSharedPtr node2 = mnet->get_node(act2,layer);

			/*if the corresponding nodes were found, and there is an edge among them, then increase the weight*/
				if(node1 && node2 && mnet->get_edge(node1,node2))
				{
					expectedWeight+=1;
				}
			}
		}
		return expectedWeight;
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
 * @brief Detect communities in multiplex network.
 * This is the implementation of Cocasa's method in detecting multi-dimentional communities using flattening
 * https://pdfs.semanticscholar.org/fb9f/ec17f5962ecbc18b49f2057cbce0447e117d.pdf
 * @mnet:  The input multiplex network instance
 * @wType: The weightening method to be implemented in the resulted edges in the flattened graph
 * @slAlgo:The single layer algorithm to be used to detect the communities
 * @return:The resulted communities
 **/

CommunityStructureSharedPtr  flattenAndDetectComs(const MLNetworkSharedPtr& mnet, WeighteningType wType,SingleLayerAlgorithm slAlgo){

 //STEPS:
 //(1) Mapping function : from multi-dimentional(multiplex) to mono-dimentional(flattened layer)
	 /*add a new layer to the same multi-layer instance*/
	LayerSharedPtr flattenedLayer =  mnet->add_layer("flattened",UNDIRECTED);
	/*add the wight attribute to the edges of this layer*/
	mnet->edge_features(flattenedLayer,flattenedLayer)->add("_WEIGHT",NUMERIC_TYPE);
	/*for each edge in the input network*/
	for (EdgeSharedPtr edge : *mnet->get_edges()) {
		/*consider the edge only if is an intra-layer edge*/
		if(edge->v1->layer == edge->v2->layer)
		{
			ActorSharedPtr act1 = edge->v1->actor;
			ActorSharedPtr act2 = edge->v2->actor;
			NodeSharedPtr from = mnet->add_node(act1,flattenedLayer) ;
			NodeSharedPtr to = mnet->add_node(act2,flattenedLayer);
			double expectedWeight=0;
			//Add the edge if the nodes are retrieved and the edge is not already existent
			if(from && to && !mnet->get_edge(from,to))
			{
				mnet->add_edge(from,to);
				switch (wType) {
						case ZeroOne: expectedWeight =1 ; break;
						case NumOfLayers:
                        expectedWeight = numOfDimentionsWeightening(mnet,act1,act2) ;break;
						case Neighborhood: expectedWeight = neighborhoodWeightening(mnet,act1,act2);
                        break;
						default: expectedWeight =1;
				}
			    mnet->set_weight(from,to,expectedWeight);
			}

		}
	  }

 //(2) Perform community detection on the resulted mono-dimentional graph (in other words, the flattened layer)
	switch (slAlgo) {
				case LabelPropagation:
					 return(label_propagation_single(mnet, flattenedLayer));
				default:
					 return (label_propagation_single(mnet, flattenedLayer));}
	return NULL;

}


}
