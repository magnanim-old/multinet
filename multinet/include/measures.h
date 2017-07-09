/*
 * measures.h
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 1.0
 *
 * Social Network Analysis measures for multiplex networks.
 * TESTED:
 * Node based: degree, neighborhood, exclusive neighborhood, relevance, exclusive relevance
 * Layer based: similarity
 *
 * TO BE CONSOLIDATED:
 * Distance based: Pareto distance, Pareto betweenness
 * Group based: modularity
 *
 * References to be added.
 */

#ifndef MLNET_MEASURES_H_
#define MLNET_MEASURES_H_

#include "datastructures.h"
#include "randomwalks.h"

namespace mlnet {

/**********************************************************************/
/** Degree  ***********************************************************/
/**********************************************************************/

long degree(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const hash_set<LayerSharedPtr>& layers, edge_mode mode);
long degree(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode);

double degree_mean(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const hash_set<LayerSharedPtr>& layers, edge_mode mode);
double degree_deviation(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const std::unordered_set<LayerSharedPtr>& layers, edge_mode mode);

hash_map<ActorSharedPtr, int> occupation(const MLNetworkSharedPtr& mnet, double teleportation, matrix<double> transitions, int num_steps);

/**********************************************************************/
/** Neighborhood ******************************************************/
/**********************************************************************/

ActorListSharedPtr neighbors(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const hash_set<LayerSharedPtr>& layers, edge_mode mode);
ActorListSharedPtr neighbors(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode);

ActorListSharedPtr xneighbors(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const hash_set<LayerSharedPtr>& layers, edge_mode mode);
ActorListSharedPtr xneighbors(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode);

double connective_redundancy(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const hash_set<LayerSharedPtr>& layers, edge_mode mode);

/**********************************************************************/
/** Layer relevance *************************************************/
/**********************************************************************/

double relevance(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const hash_set<LayerSharedPtr>& layers, edge_mode mode);
double relevance(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode);

double xrelevance(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const hash_set<LayerSharedPtr>& layers, edge_mode mode);
double xrelevance(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode);

/**********************************************************************/
/** Layer comparison ************************************************/
/**********************************************************************/

/**
 * @param mnet
 * @param layers
 * @return , or 0 if all layers have no actors.
 */
/*
double jaccard_actor(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double jaccard_edge(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double jaccard_triangle(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double jaccard_triangle(const MLNetworkSharedPtr& mnet, const hash_set<LayerSharedPtr>& layers);
double coverage_actor(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double coverage_edge(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double coverage_triangle(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double simple_matching_actor(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double simple_matching_edge(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double simple_matching_triangle(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double russell_rao_actor(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double russell_rao_edge(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double russell_rao_triangle(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double kulczynski2_actor(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double kulczynski2_edge(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double kulczynski2_triangle(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
*/
double pearson_degree(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2, edge_mode mode);
double rho_degree(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2, edge_mode mode);

// some utility functions
property_matrix<ActorSharedPtr,LayerSharedPtr,bool> actor_existence_property_matrix(const MLNetworkSharedPtr& mnet);
property_matrix<dyad,LayerSharedPtr,bool> edge_existence_property_matrix(const MLNetworkSharedPtr& mnet);
property_matrix<triad,LayerSharedPtr,bool> triangle_existence_property_matrix(const MLNetworkSharedPtr& mnet);
property_matrix<ActorSharedPtr,LayerSharedPtr,double> actor_degree_property_matrix(const MLNetworkSharedPtr& mnet, edge_mode mode);

// FROM HERE, PORTING NOT COMPLETED YET

/**********************************************************************/
/** Distances *********************************************************/
/**********************************************************************/

hash_map<ActorSharedPtr,std::set<path_length> > pareto_distance(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& from);
/*
std::map<actor_id,std::set<Path> > pareto_distance_all_paths(const MLNetworkSharedPtr& mnet, actor_id vertex);
*/

/**********************************************************************/
/** Betweenness *******************************************************/
/**********************************************************************/

/*
std::map<actor_id,long> pareto_betweenness(const MLNetworkSharedPtr& mnet);

std::map<global_edge_id, long> pareto_edge_betweenness(const MLNetworkSharedPtr& mnet);

int check_dominance(const Distance& d1, const Distance& d2);

int check_dominance(const Path& p1, const Path& p2);

*/


} // namespace mlnet

#endif /* MLNET_MEASURES_H_ */
