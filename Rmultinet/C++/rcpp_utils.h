/*
 * rcpp_utils.h
 *
 * Created on: Jul 29, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#ifndef MULTIPLENETWORK_RCPP_UTILS_H_
#define MULTIPLENETWORK_RCPP_UTILS_H_

#include "Rcpp.h"
#include "multinet.h"
#include "r_functions.h"
#include <unordered_set>
#include <vector>

using namespace Rcpp;
using namespace mlnet;

std::vector<LayerSharedPtr> resolve_layers(const MLNetworkSharedPtr& mnet, const CharacterVector& names);
std::unordered_set<LayerSharedPtr> resolve_layers_unordered(const MLNetworkSharedPtr& mnet, const CharacterVector& names);

std::vector<ActorSharedPtr> resolve_actors(const MLNetworkSharedPtr& mnet, const CharacterVector& names);
std::unordered_set<ActorSharedPtr> resolve_actors_unordered(const MLNetworkSharedPtr& mnet, const CharacterVector& names);

std::vector<EdgeSharedPtr> resolve_edges(const MLNetworkSharedPtr& mnet, const CharacterVector& edge_matrix);

std::vector<NodeSharedPtr> resolve_nodes(const MLNetworkSharedPtr& mnet, const CharacterVector& node_matrix);

edge_mode resolve_mode(std::string mode);

#endif /* MULTIPLENETWORK_RCPP_UTILS_H_ */
