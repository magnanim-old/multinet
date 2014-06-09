/*
 * exceptions.h
 *
 *  Created on: Feb 5, 2014
 *  Author: Matteo Magnani <matteo.magnani@it.uu.se>
 */

#ifndef MULTIPLENETWORK_MEASURES_H_
#define MULTIPLENETWORK_MEASURES_H_

#include <exception>
#include <string>
#include "datastructures.h"

const int PATH_DOMINATED = -1;
const int PATH_EQUAL = 0;
const int PATH_INCOMPARABLE = 1;
const int PATH_DOMINATES = 2;


/**********************************************************************/
/** Degree  ***********************************************************/
/**********************************************************************/

long out_degree(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
long out_degree(const MultiplexNetwork& mnet, const std::string& vertex, const std::set<std::string>& active_networks);
long out_degree(const MultiplexNetwork& mnet, global_identity identity, network_id network);
long out_degree(const MultiplexNetwork& mnet, const std::string& vertex, const std::string& network_name);

long in_degree(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
long in_degree(const MultiplexNetwork& mnet, const std::string& vertex, const std::set<std::string>& active_networks);
long in_degree(const MultiplexNetwork& mnet, global_identity identity, network_id network);
long in_degree(const MultiplexNetwork& mnet, const std::string& vertex, const std::string& network_name);

long degree(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
long degree(const MultiplexNetwork& mnet, const std::string& vertex, const std::set<std::string>& active_networks);
long degree(const MultiplexNetwork& mnet, global_identity identity, network_id network);
long degree(const MultiplexNetwork& mnet, const std::string& vertex, const std::string& network_name);


/**********************************************************************/
/** Neighborhood ******************************************************/
/**********************************************************************/

std::set<global_identity> out_neighbors(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
std::set<std::string> out_neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
std::set<global_identity> out_neighbors(const MultiplexNetwork& mnet, global_identity identity, network_id network);
std::set<std::string> out_neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

std::set<global_identity> in_neighbors(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
std::set<std::string> in_neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
std::set<global_identity> in_neighbors(const MultiplexNetwork& mnet, global_identity identity, network_id network);
std::set<std::string> in_neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

std::set<global_identity> neighbors(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
std::set<std::string> neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
std::set<global_identity> neighbors(const MultiplexNetwork& mnet, global_identity identity, network_id network);
std::set<std::string> neighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

std::set<global_identity> out_xneighbors(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
std::set<std::string> out_xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
std::set<global_identity> out_xneighbors(const MultiplexNetwork& mnet, global_identity identity, network_id network);
std::set<std::string> out_xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

std::set<global_identity> in_xneighbors(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
std::set<std::string> in_xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
std::set<global_identity> in_xneighbors(const MultiplexNetwork& mnet, global_identity identity, network_id network);
std::set<std::string> in_xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

std::set<global_identity> xneighbors(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
std::set<std::string> xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
std::set<global_identity> xneighbors(const MultiplexNetwork& mnet, global_identity identity, network_id network);
std::set<std::string> xneighbors(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

/**********************************************************************/
/** Network relevance *************************************************/
/**********************************************************************/

double out_relevance(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
double out_relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
double out_relevance(const MultiplexNetwork& mnet, global_identity identity, network_id network);
double out_relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

double in_relevance(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
double in_relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
double in_relevance(const MultiplexNetwork& mnet, global_identity identity, network_id network);
double in_relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

double relevance(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
double relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
double relevance(const MultiplexNetwork& mnet, global_identity identity, network_id network);
double relevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

double out_xrelevance(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
double out_xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
double out_xrelevance(const MultiplexNetwork& mnet, global_identity identity, network_id network);
double out_xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

double in_xrelevance(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
double in_xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
double in_xrelevance(const MultiplexNetwork& mnet, global_identity identity, network_id network);
double in_xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

double xrelevance(const MultiplexNetwork& mnet, global_identity identity, const std::set<network_id>& active_networks);
double xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::set<std::string>& active_networks);
double xrelevance(const MultiplexNetwork& mnet, global_identity identity, network_id network);
double xrelevance(const MultiplexNetwork& mnet, const std::string& global_name, const std::string& network_name);

/**********************************************************************/
/** Distances *********************************************************/
/**********************************************************************/

//void pareto_distance(MultipleNetwork& mnet, global_identity identity, std::map<global_identity,Distance>& distances);

void pareto_distance_single_paths(const MultiplexNetwork& mnet, global_identity identity, std::vector<std::set<Path> >& paths);

void pareto_distance_all_paths(const MultiplexNetwork& mnet, global_identity identity, std::map<global_identity,std::set<Path> >& paths, int bound);

void pareto_distance_all_paths(const MultiplexNetwork& mnet, global_identity identity, std::map<global_identity,std::set<Path> >& paths);

/**********************************************************************/
/** Betweenness *******************************************************/
/**********************************************************************/

void pareto_betweenness(const MultiplexNetwork& mnet, std::map<global_identity,long>& vertex_betweenness, int bound);

void pareto_betweenness(const MultiplexNetwork& mnet, std::map<global_identity,long>& vertex_betweenness);

void pareto_edge_betweenness(const MultiplexNetwork& mnet, std::map<global_edge_id, long>& edge_betweenness);

int check_dominance(const Path& p1, const Path& p2);

/**********************************************************************/
/** Clustering ********************************************************/
/**********************************************************************/

double modularity(const MultiplexNetwork& mnet, std::map<network_id,std::map<global_identity,long> >& communities, double c);

#endif /* MULTIPLENETWORK_MEASURES_H_ */
