/*
 * measures.h
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: beta
 *
 * Social Network Analysis measures for multiplex networks.
 * TESTED:
 * Node based: degree, neighborhood, exclusive neighborhood, network relevance, exclusive network relevance
 * Network based: similarity
 *
 * TO BE CONSOLIDATED:
 * Distance based: Pareto distance, Pareto betweenness
 * Group based: modularity
 *
 * References to be added.
 */

#ifndef MULTIPLENETWORK_MEASURES_H_
#define MULTIPLENETWORK_MEASURES_H_

#include <exception>
#include <string>
#include "datastructures.h"

namespace mlnet {

const int P_DOMINATED = -1;
const int P_EQUAL = 0;
const int P_INCOMPARABLE = 1;
const int P_DOMINATES = 2;

/**********************************************************************/
/** Degree  ***********************************************************/
/**********************************************************************/

long degree(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode);
long degree(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode);

double degree_mean(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode);
double degree_deviation(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode);

/**********************************************************************/
/** Neighborhood ******************************************************/
/**********************************************************************/

ObjectStore<ActorSharedPtr> neighbors(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode);
ObjectStore<ActorSharedPtr> neighbors(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode);

ObjectStore<ActorSharedPtr> xneighbors(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode);
ObjectStore<ActorSharedPtr> xneighbors(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode);

/**********************************************************************/
/** Layer relevance *************************************************/
/**********************************************************************/

double relevance(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode);
double relevance(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode);

double xrelevance(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode);
double xrelevance(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode);

/**********************************************************************/
/** Network comparison ************************************************/
/**********************************************************************/

double network_jaccard_similarity(const MLNetworkSharedPtr mnet, const std::set<LayerSharedPtr>& layers);
double network_jaccard_similarity(const MLNetworkSharedPtr mnet, const std::set<std::string>& active_networks);
double network_jaccard_similarity(const MLNetworkSharedPtr mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double network_jaccard_similarity(const MLNetworkSharedPtr mnet, const std::string& network_name1, const std::string& network_name2);

double network_coverage(const MLNetworkSharedPtr mnet, const std::set<layer_id>& n1, const std::set<layer_id>& n2);
double network_coverage(const MLNetworkSharedPtr mnet, const std::set<std::string>& n1, const std::set<std::string>& n2);
double network_coverage(const MLNetworkSharedPtr mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
double network_coverage(const MLNetworkSharedPtr mnet, const std::string& network_name1, const std::string& network_name2);

// FROM HERE, PORTING NOT COMPLETED YET

/**********************************************************************/
/** Distances *********************************************************/
/**********************************************************************/

//void pareto_distance(MultipleNetwork& mnet, global_identity identity, std::map<global_identity,Distance>& distances);

/*
std::map<actor_id,std::set<Distance> > pareto_distance(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor,);

std::map<actor_id,std::set<Path> > pareto_distance_all_paths(const MLNetworkSharedPtr mnet, actor_id vertex);
*/

/**********************************************************************/
/** Betweenness *******************************************************/
/**********************************************************************/

/*
std::map<actor_id,long> pareto_betweenness(const MLNetworkSharedPtr mnet);

std::map<global_edge_id, long> pareto_edge_betweenness(const MLNetworkSharedPtr mnet);

int check_dominance(const Distance& d1, const Distance& d2);

int check_dominance(const Path& p1, const Path& p2);

*/

/**********************************************************************/
/** Clustering ********************************************************/
/**********************************************************************/

double modularity(const MLNetworkSharedPtr mnet, std::map<layer_id,std::map<actor_id,long> >& communities, double c);

//std::vector<long> distribution(long (*f)(const MLNetworkSharedPtr, global_identity, const std::set<layer_id>&));

} // namespace mlnet

#endif /* MULTIPLENETWORK_MEASURES_H_ */
