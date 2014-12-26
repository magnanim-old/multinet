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

const int P_DOMINATED = -1;
const int P_EQUAL = 0;
const int P_INCOMPARABLE = 1;
const int P_DOMINATES = 2;


/**********************************************************************/
/** Degree  ***********************************************************/
/**********************************************************************/

long out_degree(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
long out_degree(const MultiplexNetwork& mnet, const std::string& vertex, const std::set<std::string>& active_networks);
long out_degree(const MultiplexNetwork& mnet, entity_id identity, network_id network);
long out_degree(const MultiplexNetwork& mnet, const std::string& vertex, const std::string& network_name);

long in_degree(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
long in_degree(const MultiplexNetwork& mnet, const std::string& vertex, const std::set<std::string>& active_networks);
long in_degree(const MultiplexNetwork& mnet, entity_id identity, network_id network);
long in_degree(const MultiplexNetwork& mnet, const std::string& vertex, const std::string& network_name);

long degree(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
long degree(const MultiplexNetwork& mnet, const std::string& vertex, const std::set<std::string>& active_networks);
long degree(const MultiplexNetwork& mnet, entity_id identity, network_id network);
long degree(const MultiplexNetwork& mnet, const std::string& vertex, const std::string& network_name);


/**********************************************************************/
/** Neighborhood ******************************************************/
/**********************************************************************/

std::set<entity_id> out_neighbors(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
std::set<std::string> out_neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
std::set<entity_id> out_neighbors(const MultiplexNetwork& mnet, entity_id identity, network_id network);
std::set<std::string> out_neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

std::set<entity_id> in_neighbors(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
std::set<std::string> in_neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
std::set<entity_id> in_neighbors(const MultiplexNetwork& mnet, entity_id identity, network_id network);
std::set<std::string> in_neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

std::set<entity_id> neighbors(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
std::set<std::string> neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
std::set<entity_id> neighbors(const MultiplexNetwork& mnet, entity_id identity, network_id network);
std::set<std::string> neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

std::set<entity_id> out_xneighbors(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
std::set<std::string> out_xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
std::set<entity_id> out_xneighbors(const MultiplexNetwork& mnet, entity_id identity, network_id network);
std::set<std::string> out_xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

std::set<entity_id> in_xneighbors(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
std::set<std::string> in_xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
std::set<entity_id> in_xneighbors(const MultiplexNetwork& mnet, entity_id identity, network_id network);
std::set<std::string> in_xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

std::set<entity_id> xneighbors(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
std::set<std::string> xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
std::set<entity_id> xneighbors(const MultiplexNetwork& mnet, entity_id identity, network_id network);
std::set<std::string> xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

/**********************************************************************/
/** Network relevance *************************************************/
/**********************************************************************/

double out_relevance(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
double out_relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
double out_relevance(const MultiplexNetwork& mnet, entity_id identity, network_id network);
double out_relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

double in_relevance(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
double in_relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
double in_relevance(const MultiplexNetwork& mnet, entity_id identity, network_id network);
double in_relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

double relevance(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
double relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
double relevance(const MultiplexNetwork& mnet, entity_id identity, network_id network);
double relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

double out_xrelevance(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
double out_xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
double out_xrelevance(const MultiplexNetwork& mnet, entity_id identity, network_id network);
double out_xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

double in_xrelevance(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
double in_xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
double in_xrelevance(const MultiplexNetwork& mnet, entity_id identity, network_id network);
double in_xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

double xrelevance(const MultiplexNetwork& mnet, entity_id identity, const std::set<network_id>& active_networks);
double xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
double xrelevance(const MultiplexNetwork& mnet, entity_id identity, network_id network);
double xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

/**********************************************************************/
/** Network comparison ************************************************/
/**********************************************************************/

double network_jaccard_similarity(const MultiplexNetwork& mnet, const std::set<network_id>& active_networks);
double network_jaccard_similarity(const MultiplexNetwork& mnet, const std::set<std::string>& active_networks);
double network_jaccard_similarity(const MultiplexNetwork& mnet, network_id network1, network_id network2);
double network_jaccard_similarity(const MultiplexNetwork& mnet, const std::string& network_name1, const std::string& network_name2);

double network_coverage(const MultiplexNetwork& mnet, const std::set<network_id>& n1, const std::set<network_id>& n2);
double network_coverage(const MultiplexNetwork& mnet, const std::set<std::string>& n1, const std::set<std::string>& n2);
double network_coverage(const MultiplexNetwork& mnet, network_id network1, network_id network2);
double network_coverage(const MultiplexNetwork& mnet, const std::string& network_name1, const std::string& network_name2);

// FROM HERE, PORTING NOT COMPLETED YET

/**********************************************************************/
/** Distances *********************************************************/
/**********************************************************************/

//void pareto_distance(MultipleNetwork& mnet, global_identity identity, std::map<global_identity,Distance>& distances);

std::map<entity_id,std::set<Distance> > pareto_distance(const MultiplexNetwork& mnet, entity_id identity);

std::map<entity_id,std::set<Path> > pareto_distance_all_paths(const MultiplexNetwork& mnet, entity_id vertex);

/**********************************************************************/
/** Betweenness *******************************************************/
/**********************************************************************/

std::map<entity_id,long> pareto_betweenness(const MultiplexNetwork& mnet);

std::map<global_edge_id, long> pareto_edge_betweenness(const MultiplexNetwork& mnet);

int check_dominance(const Distance& d1, const Distance& d2);

int check_dominance(const Path& p1, const Path& p2);

/**********************************************************************/
/** Clustering ********************************************************/
/**********************************************************************/

double modularity(const MultiplexNetwork& mnet, std::map<network_id,std::map<entity_id,long> >& communities, double c);

//std::vector<long> distribution(long (*f)(const MultiplexNetwork&, global_identity, const std::set<network_id>&));


#endif /* MULTIPLENETWORK_MEASURES_H_ */
