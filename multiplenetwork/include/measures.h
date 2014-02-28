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

long out_degree(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks);
long out_degree(MultipleNetwork& mnet, std::string vertex, std::set<std::string> active_networks);
long out_degree(MultipleNetwork& mnet, vertex_id vertex, network_id network);
long out_degree(MultipleNetwork& mnet, std::string vertex, std::string network_name);

long in_degree(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks);
long in_degree(MultipleNetwork& mnet, std::string vertex, std::set<std::string> active_networks);
long in_degree(MultipleNetwork& mnet, vertex_id vertex, network_id network);
long in_degree(MultipleNetwork& mnet, std::string vertex, std::string network_name);

long degree(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks);
long degree(MultipleNetwork& mnet, std::string vertex, std::set<std::string> active_networks);
long degree(MultipleNetwork& mnet, vertex_id vertex, network_id network);
long degree(MultipleNetwork& mnet, std::string vertex, std::string network_name);


/**********************************************************************/
/** Neighborhood ******************************************************/
/**********************************************************************/

std::set<vertex_id> out_neighbors(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks);
std::set<vertex_id> out_neighbors(MultipleNetwork& mnet, vertex_id vertex, std::set<std::string> active_networks);
std::set<vertex_id> out_neighbors(MultipleNetwork& mnet, vertex_id vertex, network_id network);
std::set<vertex_id> out_neighbors(MultipleNetwork& mnet, vertex_id vertex, std::string network_name);

std::set<vertex_id> in_neighbors(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks);
std::set<vertex_id> in_neighbors(MultipleNetwork& mnet, vertex_id vertex, std::set<std::string> active_networks);
std::set<vertex_id> in_neighbors(MultipleNetwork& mnet, vertex_id vertex, network_id network);
std::set<vertex_id> in_neighbors(MultipleNetwork& mnet, vertex_id vertex, std::string network_name);

std::set<vertex_id> out_neighbors_xor(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks);
std::set<vertex_id> out_neighbors_xor(MultipleNetwork& mnet, vertex_id vertex, std::set<std::string> active_networks);
std::set<vertex_id> out_neighbors_xor(MultipleNetwork& mnet, vertex_id vertex, network_id network);
std::set<vertex_id> out_neighbors_xor(MultipleNetwork& mnet, vertex_id vertex, std::string network_name);

std::set<vertex_id> in_neighbors_xor(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks);
std::set<vertex_id> in_neighbors_xor(MultipleNetwork& mnet, vertex_id vertex, std::set<std::string> active_networks);
std::set<vertex_id> in_neighbors_xor(MultipleNetwork& mnet, vertex_id vertex, network_id network);
std::set<vertex_id> in_neighbors_xor(MultipleNetwork& mnet, vertex_id vertex, std::string network_name);

/**********************************************************************/
/** Network relevance *************************************************/
/**********************************************************************/

std::set<vertex_id> out_relevance(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks);
std::set<vertex_id> out_relevance(MultipleNetwork& mnet, vertex_id vertex, std::set<std::string> active_networks);
std::set<vertex_id> out_relevance(MultipleNetwork& mnet, vertex_id vertex, network_id network);
std::set<vertex_id> out_relevance(MultipleNetwork& mnet, vertex_id vertex, std::string network_name);

std::set<vertex_id> in_relevance(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks);
std::set<vertex_id> in_relevance(MultipleNetwork& mnet, vertex_id vertex, std::set<std::string> active_networks);
std::set<vertex_id> in_relevance(MultipleNetwork& mnet, vertex_id vertex, network_id network);
std::set<vertex_id> in_relevance(MultipleNetwork& mnet, vertex_id vertex, std::string network_name);

std::set<vertex_id> out_relevance_xor(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks);
std::set<vertex_id> out_relevance_xor(MultipleNetwork& mnet, vertex_id vertex, std::set<std::string> active_networks);
std::set<vertex_id> out_relevance_xor(MultipleNetwork& mnet, vertex_id vertex, network_id network);
std::set<vertex_id> out_relevance_xor(MultipleNetwork& mnet, vertex_id vertex, std::string network_name);

std::set<vertex_id> in_relevance_xor(MultipleNetwork& mnet, vertex_id vertex, std::set<network_id> active_networks);
std::set<vertex_id> in_relevance_xor(MultipleNetwork& mnet, vertex_id vertex, std::set<std::string> active_networks);
std::set<vertex_id> in_relevance_xor(MultipleNetwork& mnet, vertex_id vertex, network_id network);
std::set<vertex_id> in_relevance_xor(MultipleNetwork& mnet, vertex_id vertex, std::string network_name);

/**********************************************************************/
/** Distances *********************************************************/
/**********************************************************************/

//void pareto_distance(MultipleNetwork& mnet, vertex_id vertex, std::map<vertex_id,Distance>& distances);

void pareto_distance_single_paths(MultipleNetwork& mnet, vertex_id vertex, std::vector<std::set<Path> >& paths);

void pareto_distance_all_paths(MultipleNetwork& mnet, vertex_id vertex, std::map<vertex_id,std::set<Path> >& paths);

/**********************************************************************/
/** Betweenness *******************************************************/
/**********************************************************************/

void pareto_betweenness(MultipleNetwork& mnet, std::map<vertex_id,long>& vertex_betweenness);

void pareto_edge_betweenness(MultipleNetwork& mnet, std::map<edge, long>& edge_betweenness);

int check_dominance(const Path& p1, const Path& p2);

/**********************************************************************/
/** Clustering ********************************************************/
/**********************************************************************/

double modularity(MultipleNetwork& mnet, std::map<network_id,std::map<vertex_id,long> >& communities, double c);

#endif /* MULTIPLENETWORK_MEASURES_H_ */
