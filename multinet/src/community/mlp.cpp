#include "utils.h"
#include "community.h"
#include "utils/sortedrandomset.h"
#include <iostream>
#include <fstream>


namespace mlnet {



/**
* @brief return the intersection of two sets of layers S1, S2
 * @first : the first set.
 * @second : the second set.
 * @return : the intersection of the two sets
 **/
vector<LayerSharedPtr> get_intersection(const vector<LayerSharedPtr>& first,const vector<LayerSharedPtr>& second){

	vector<LayerSharedPtr> result;
	for(LayerSharedPtr layer:first){
		if(std::find(second.begin(),second.end(),layer)!=second.end()){
			result.push_back(layer);
		}
	}
	return result;

}

/**
 * @brief return the union of two sets of layers S1, S2
 * @first : the first set.
 * @second : the second set.
 * @return : the union of the two sets
 **/

vector<LayerSharedPtr> get_union(const vector<LayerSharedPtr>& first,const vector<LayerSharedPtr>& second){

	vector<LayerSharedPtr> result;
	for(LayerSharedPtr layer:first){
		result.push_back(layer);
	}
	for(LayerSharedPtr layer:second){
		if(std::find(result.begin(),result.end(),layer)==result.end()){
			result.push_back(layer);
		}
	}
	return result;

}

/**
 * @brief check if a set  of layers S1 is a subset of another set S2 of layers
 * @subset : set to be checked.
 * @super_set : the set assumed to be the super set
 * @return : true if "subset" is a subset of "superset", false otherwise
 **/

bool is_subset(vector<LayerSharedPtr> subset,vector<LayerSharedPtr> super_set){
	for(LayerSharedPtr layer:subset){
		//if the an element in the subset is not found in the superset
		if(std::find(super_set.begin(),super_set.end(),layer)==super_set.end()){
			return false ;
		}
	}
	return true;
}

/**
 * @brief calculates the sum of initial attraction weights among an actor and his neighbours given a set of layers L.
 * the neighbours considered in this calculation are just the neighbours that are his neighbours in all the layers in L.
 * @layers_set : set of layers.
 * @actr : an actor
 * @actor_neighbours : the actor neighbours in all layers.
 * attr_weights : the initial attraction weights w0.
 * @return : sum of attraction weights (Sum(w0)) between "actr" and his neighbours in "layers_set".
 **/

double get_sum_of_w0(const vector<LayerSharedPtr>& layers_set,
									 const ActorSharedPtr& actr,
									 const hash_map<LayerSharedPtr,vector<ActorSharedPtr>>& actor_neighbours,
									 const hash_map<ActorSharedPtr,hash_map<ActorSharedPtr,float>>& attr_weights){


	double sum_of_w0=0;

	//find the set of actors that are neighbours with "actr" in all the layers given in "layers_set"
	vector<ActorSharedPtr> intersection = actor_neighbours.begin()->second;
	for(LayerSharedPtr layer:layers_set){
			vector<ActorSharedPtr> first_set = intersection;
			intersection.clear();
			for(ActorSharedPtr actor_in_set_2:actor_neighbours.at(layer)){
					if(std::find(first_set.begin(),first_set.end(),actor_in_set_2)!=first_set.end()){
						intersection.push_back(actor_in_set_2);
					}
				}

	}
	//calculate the sum of initial attraction weights among "actr" and the neighbours selected in the previous step
	if(intersection.size()!=0){
		for(ActorSharedPtr neighbour:intersection){
			sum_of_w0 += attr_weights.at(actr).at(neighbour);
		}
	}

	return sum_of_w0;
}

/**
 * @brief evaluate the relevance of a group of layers to an actor "act".
 * This is called "Dimensions relevance" (according to Berlingerio) and it is a ratio of the actor neighbours that can be
 * exclusively reached within the given set of dimensions (layers) to the total number of actor neighbours in all dimensions (layers)
 * @mnet : the multi-layer network instance
 * @dims : a subset of layers (dimensions)
 * @act : an actor
 * @return : a real number [0..1] reflecting the relevance of the given "dimensions" to the actor.
 **/

double get_dimentions_relevance_for_actor(const vector<LayerSharedPtr>& dims,
								 const hash_map<ActorSharedPtr,vector<LayerSharedPtr>>& actor_neighbours){



	//count the number of actors that are exclusively neighbours with "actr" within "dims"
	int num_of_all_neighbours=0 ;
	int num_of_exclusive_neighbours = 0;
	for(hash_map<ActorSharedPtr,vector<LayerSharedPtr>>::const_iterator itr = actor_neighbours.begin();itr!=actor_neighbours.end();++itr){
	 	num_of_all_neighbours++;
	    if(is_subset(itr->second,dims)) num_of_exclusive_neighbours++;
	}
	double relevance = (double)(num_of_exclusive_neighbours)/num_of_all_neighbours;
	return relevance;

}

/**
 * @brief recover the actor community structure given their memberships
 * @membership : the memberships of the actors
 * @return : actor communities
 **/

ActorCommunityStructureSharedPtr to_community_structure(hash_map<ActorSharedPtr,int> membership){

	ActorCommunityStructureSharedPtr result = actor_community_structure::create();
	hash_map<int,hash_set<ActorSharedPtr> > communities;
	for (auto pair: membership) {
	     communities[pair.second].insert(pair.first);
	}
	for (auto pair: communities) {
	     ActorCommunitySharedPtr c = actor_community::create();
	     for (ActorSharedPtr actor: pair.second)
	         c->add_actor(actor);
	     result->add_community(c);
	}
	return result;
}

/**
 * @brief find communities in a multiplex using the multi-layer label propagation algorithm.
 * reference https://dl.acm.org/citation.cfm?id=3080574
 * @mnet : the multi-layer network instance
 * @return : actor communities
 **/

ActorCommunityStructureSharedPtr  mlp(const MLNetworkSharedPtr& mnet){

	/*(1) calculate the initinal attraction weights w0 for actors (affinity of actors to their neighbours)*/

		//initialization
		hash_map<ActorSharedPtr,hash_map<ActorSharedPtr,float>> initial_attraction_weights; //w0 in the article
		hash_map<ActorSharedPtr,hash_map<ActorSharedPtr,float>> updated_attraction_weights; //w in the article
		hash_map<ActorSharedPtr,hash_map<ActorSharedPtr,vector<LayerSharedPtr>>> actors_shared_layers; //store the layers where the actors are neighbours (D(v,u) in the article)
		hash_map<ActorSharedPtr,vector<LayerSharedPtr>> actors_relevant_dimensions; //Dv in the article
		hash_map<ActorSharedPtr,hash_map<LayerSharedPtr,vector<ActorSharedPtr>>> all_actors_neighbours; //to store the (actors & layers) references of an actor where the actor has neighbours

		//for each actor, calculate its affinity to each of its neighbours
		for(ActorSharedPtr actor:*mnet->get_actors()){
			//get actor's nodes in the multiplex layers
			NodeListSharedPtr actor_nodes = mnet->get_nodes(actor);
			//find the neighbours of all the nodes (the neighbours of an actor in all layers)
				hash_map<ActorSharedPtr,vector<LayerSharedPtr>> actor_neighbours;
				for(NodeSharedPtr node:*actor_nodes){
					//get node's neighbours
					NodeListSharedPtr node_neighbours = mnet->neighbors(node,INOUT);
			        if(node_neighbours->size()!=0)
			        {
			        	//add the neighbour as "key" and its "layer" to the "value" vector (if it is not already existent in the vector)
			        	for(NodeSharedPtr neighbr:*node_neighbours){
			        		if(std::find(actor_neighbours[neighbr->actor].begin(),actor_neighbours[neighbr->actor].end(),neighbr->layer)==actor_neighbours[neighbr->actor].end()) {
			        			actor_neighbours[neighbr->actor].push_back(neighbr->layer);
			        			}
			        		//update the neighbours map as well.
			        		all_actors_neighbours[actor][neighbr->layer].push_back(neighbr->actor);

			        	}
			        }
				}

			//calculate the initial affinity of this actor to each of its neighbours
			for(auto pair:actor_neighbours){
				actors_shared_layers[actor][pair.first]=actor_neighbours[pair.first];
				//calculate the dimensions relevance of the current actor  with respect to the common layers D(v,u) he shares with "neighbour"
				double affinity = get_dimentions_relevance_for_actor(actors_shared_layers[actor][pair.first],actor_neighbours);
				initial_attraction_weights[actor][pair.first]=affinity;
			}
		}

	/*(2) recover the relevant dimensions (layers) Dv for each actor using equation(8) in the reference*/
		for(ActorSharedPtr actor:*mnet->get_actors()){
			//retrieve the layers in which the actors has neighbours
			vector<LayerSharedPtr> activ_in_layers;
			for(auto pair:all_actors_neighbours.at(actor)){
				activ_in_layers.push_back(pair.first);
			}

			//calculate the sum of w0 for neighbours within all subsets of layers in which the actor is active
			double max_sum_of_w0=0;
			vector<LayerSharedPtr> subset;
			vector<LayerSharedPtr> actor_relevant_dimentions;
			for(size_t i=0;i<activ_in_layers.size();i++){
				subset.clear();
				for(size_t j=i;j<activ_in_layers.size();j++){
					//this is a possible subset of layers now
					subset.push_back(activ_in_layers[j]);
					//get the sum of initial attraction weights within this subset of layers
					double sum_of_w0 = get_sum_of_w0(subset,actor,all_actors_neighbours[actor],initial_attraction_weights);
					if(sum_of_w0 > max_sum_of_w0 ){
						max_sum_of_w0=sum_of_w0;
						actor_relevant_dimentions = subset;
					}
					else if (sum_of_w0 == max_sum_of_w0)
					{
						actor_relevant_dimentions = get_union(actor_relevant_dimentions,subset);
					}
				}
			}
			actors_relevant_dimensions[actor] =actor_relevant_dimentions;
		}

	/*(3)  calculate the new attraction weights w for actors (equation (7) in the article)*/
	for(auto pair:initial_attraction_weights){
	 for(auto sub_pair: pair.second){
		 ActorSharedPtr act1 = pair.first;
		 ActorSharedPtr act2 = sub_pair.first;
		 vector<LayerSharedPtr> intersect  = get_intersection(actors_relevant_dimensions[act2],actors_shared_layers[act1][act2]);
		 vector<LayerSharedPtr> union_= get_union(actors_relevant_dimensions[act2],actors_shared_layers[act1][act2]);
		 updated_attraction_weights[act1][act2]=initial_attraction_weights[act1][act2]*((double)intersect.size()/union_.size());
	  }
	}

	 /*(4)The labeling step*/
	 ActorListSharedPtr actors = mnet->get_actors();
	 hash_map<ActorSharedPtr,int> membership; // community membership
	 std::vector<ActorSharedPtr> order; //order vector to decide in which order to process the actors

	 //Initialize labels
	 int label=0;
	 for (ActorSharedPtr actor: *actors) {
	   membership[actor] = label;
	   order.push_back(actor);
	   label++;
	 }
	 //keep updating the labels as long as the stopping condition is not satisfied
	 int i=0;
	 while (i<10) {

	            /* shuffle the order of the actors */
	            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	            std::shuffle(order.begin(), order.end(), std::default_random_engine(seed));

	            /* re-assign labels */
	            for (ActorSharedPtr actor: order) {
	            	//group the neighbours according to their labels calculate the sum of w within each group
	            	hash_map<int,double> neigh_groups_wieghts;
	            	for(auto pair:updated_attraction_weights[actor]){
	            		neigh_groups_wieghts[membership.at(pair.first)]+=updated_attraction_weights[actor][pair.first];
	            	}
	            	//find the label in the group of neighbours that has the maximum sum of w
	            	int winning_label = neigh_groups_wieghts.begin()->first;
	            	double max = neigh_groups_wieghts.begin()->second;
	            	for(auto pair:neigh_groups_wieghts){
	            		if(pair.second >max){
	            			winning_label=pair.first;
	            			max=pair.second;
	            		}
	            	}
	            	//assign the winning label to the actor
	            	membership[actor]=winning_label;

	            	//update the relevant dimensions Dv for this actor according to equation (10) in the article
	            	vector<LayerSharedPtr> union_Dvu;
	            	vector<LayerSharedPtr> union_Du;
	            	for(hash_map<ActorSharedPtr,vector<LayerSharedPtr>>::iterator itr=actors_shared_layers[actor].begin();itr!=actors_shared_layers[actor].end();++itr){
	            		if(membership[itr->first]==winning_label){
	            			union_Dvu = get_union(union_Dvu,itr->second);
	            			union_Du = get_union(union_Du,actors_relevant_dimensions[itr->first]);
	            		}
	            	}
	            	actors_relevant_dimensions[actor]=get_intersection(union_Dvu,union_Du);

	            	//update the attraction weights according to equation(9) in the article.
	            	 for(auto pair:updated_attraction_weights[actor]){
	            			 ActorSharedPtr neighbour = pair.first;
	            			 vector<LayerSharedPtr> intersect  = get_intersection(actors_relevant_dimensions[actor],actors_shared_layers[actor][neighbour]);
	            			 vector<LayerSharedPtr> union_= get_union(actors_relevant_dimensions[actor],actors_shared_layers[actor][neighbour]);
	            			 updated_attraction_weights[neighbour][actor]=updated_attraction_weights[neighbour][actor]*((double)intersect.size()/union_.size());
	            	 }

	            }

	         //check the stopping condition
	          for (ActorSharedPtr actor: order) {
	        	  //group the neighbours according to their labels calculate the sum of w within each group
					hash_map<int,double> neigh_groups_wieghts;
					for(auto pair:updated_attraction_weights[actor]){
						neigh_groups_wieghts[membership.at(pair.first)]+=updated_attraction_weights[actor][pair.first];
					}
					//find the label in the group of neighbours that has the maximum sum of w
					int winning_label = neigh_groups_wieghts.begin()->first;
					double max = neigh_groups_wieghts.begin()->second;
					vector<int> winning_groups_labels;
					for(auto pair:neigh_groups_wieghts){
						if(pair.second >= max){
							if(std::find(winning_groups_labels.begin(),winning_groups_labels.end(),winning_label)==winning_groups_labels.end()){
								winning_groups_labels.push_back(winning_label);}

						}
						if(pair.second >max){
							winning_label=pair.first;
							max=pair.second;
						}
					}
					if(std::find(winning_groups_labels.begin(),winning_groups_labels.end(),membership[actor])==winning_groups_labels.end()){
						continue;
					}
	          }
	          break;
	    }

   return to_community_structure(membership);
  }

}

