/*
 * r_functions.h
 *
 * Created on: Jun 19, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#ifndef _R_FUNCTIONS_H_
#define _R_FUNCTIONS_H_

#include <Rcpp.h>
#include "multinet.h"
#include <unordered_set>
#include <vector>

using namespace Rcpp;
using namespace mlnet;

class RMLNetwork {
	private: MLNetworkSharedPtr ptr;
	public:
	std::string name() const {return ptr->name;}
	RMLNetwork(MLNetworkSharedPtr ptr) : ptr(ptr) {}
	MLNetworkSharedPtr get_mlnet() const {return ptr;}
};

class REvolutionModel {
	private:
		EvolutionModelSharedPtr ptr;
		std::string desc;
	public:
	std::string description() const {return desc;}
	REvolutionModel(EvolutionModelSharedPtr ptr, const std::string description) : ptr(ptr), desc(description) {}
	EvolutionModelSharedPtr get_model() const {return ptr;}
};

// CREATION AND STORAGE

RMLNetwork emptyMultilayer(const std::string& name);
void renameMultilayer(RMLNetwork& rmnet, const std::string& new_name);
RMLNetwork readMultilayer(const std::string& input_file, const std::string& name, char sep);
void writeMultilayer(const RMLNetwork& mnet, const std::string& output_file, const std::string& format, const CharacterVector& layer_names, char sep, bool merge_actors, bool all_actors);

REvolutionModel ba_evolution_model(int m0, int m);
REvolutionModel er_evolution_model(int n);
RMLNetwork growMultiplex(int num_actors, long num_of_steps, const GenericVector& evolution_model, const NumericVector& pr_internal_event, const NumericVector& pr_external_event, const NumericMatrix& dependency);

// INFORMATION ON NETWORKS

CharacterVector layers(const RMLNetwork& mnet);
CharacterVector actors(const RMLNetwork& mnet, const CharacterVector& layer_names);
CharacterMatrix nodes(const RMLNetwork& mnet, const CharacterVector& layer_names);
CharacterMatrix edges(const RMLNetwork& mnet, const CharacterVector& layer_names1, const CharacterVector& layer_names2);

DataFrame edges_idx(const RMLNetwork& rmnet);

int numLayers(const RMLNetwork& mnet);
long numActors(const RMLNetwork& mnet, const CharacterVector& layers);
long numNodes(const RMLNetwork& mnet, const CharacterVector& layers);
long numEdges(const RMLNetwork& mnet, const CharacterVector& layer_names1, const CharacterVector& layer_names2);
CharacterMatrix isDirected(const RMLNetwork& mnet, const CharacterVector& layer_names1, const CharacterVector& layer_names2);

std::unordered_set<std::string> actor_neighbors(const RMLNetwork& rmnet, const std::string& actor_name, const CharacterVector& layer_names, const std::string& mode_name);
std::unordered_set<std::string> actor_xneighbors(const RMLNetwork& rmnet, const std::string& actor_name, const CharacterVector& layer_names, const std::string& mode_name);

// NETWORK MANIPULATION

void addLayers(RMLNetwork& rmnet, const CharacterVector& layer_names, const LogicalVector& directed);
void addActors(RMLNetwork& rmnet, const CharacterVector& actor_names);
void addNodes(RMLNetwork& rmnet, const CharacterVector& nodes);
void addEdges(RMLNetwork& rmnet, const CharacterVector& edges);
void setDirected(const RMLNetwork& rmnet, const CharacterVector& layers, bool directed);

void deleteLayers(RMLNetwork& rmnet, const CharacterVector& layer_names);
void deleteActors(RMLNetwork& rmnet, const CharacterVector& actor_names);
void deleteNodes(RMLNetwork& rmnet, const CharacterVector& nodes);
void deleteEdges(RMLNetwork& rmnet, const CharacterVector& edges);


void newAttributes(RMLNetwork& rmnet, const CharacterVector& attribute_names, const std::string& type, const std::string& target, const std::string& layer_name, const std::string& layer_name1, const std::string& layer_name2);
GenericMatrix getAttributes(const RMLNetwork& rmnet, const std::string& target);
GenericMatrix getValues(RMLNetwork& rmnet, const std::string& attribute_name, const CharacterVector& actor_names, const CharacterVector& layer_names, const CharacterVector& node_matrix, const CharacterVector& edge_matrix);
void setValues(RMLNetwork& rmnet, const std::string& attribute_name, const CharacterVector& actor_names, const CharacterVector& layer_names, const CharacterVector& node_matrix, const CharacterVector& edge_matrix, const GenericVector& values);

// TRANSFORMATION

void flatten(RMLNetwork& rmnet, const std::string& new_layer, const CharacterVector& layer_names, const std::string& method, bool force_directed, bool all_actors);
void project(RMLNetwork& rmnet, const std::string& new_layer, const std::string& layer1, const std::string& layer2, const std::string& method);

// MEASURES

NumericVector degree_ml(const RMLNetwork& rmnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type);
NumericVector degree_deviation_ml(const RMLNetwork& rmnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type);
NumericVector occupation_ml(const RMLNetwork& rmnet, const NumericMatrix& transitions, double teleportation, long steps);
NumericVector neighborhood_ml(const RMLNetwork& mnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type);
NumericVector xneighborhood_ml(const RMLNetwork& mnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type);
NumericVector connective_redundancy_ml(const RMLNetwork& mnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type);
NumericVector relevance_ml(const RMLNetwork& mnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type);
NumericVector xrelevance_ml(const RMLNetwork& mnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type);
double similarity_ml(const RMLNetwork& rmnet, const std::string& layer1, const std::string& layer2, const std::string& method);
double correlation_ml(const RMLNetwork& rmnet, const std::string& layer1, const std::string& layer2, const std::string& method, const std::string& type);
GenericMatrix distance_ml(const RMLNetwork& mnet, const std::string& from, const CharacterVector& to, const std::string& method);

// CLUSTERING

//GenericMatrix cliquepercolation_ml(const RMLNetwork& mnet, int k, int m1, int m2, int m3);

// Layout

DataFrame multiforce_ml(const RMLNetwork& mnet, const NumericVector& w_in, const NumericVector& w_out, const NumericVector& gravity, int iterations);


// Preprocessing
/*
void flatten_w(RMLNetworkSharedPtr& mnet, const std::string& new_layer_name, const CharacterVector& layers, bool force_directed, bool force_actors);
void flatten_or(RMLNetworkSharedPtr& mnet, const std::string& new_layer_name, const CharacterVector& layers, bool force_directed, bool force_actors);
void project_or(RMLNetworkSharedPtr& mnet, const std::string& new_layer_name, const std::string& layer1, const std::string& layer2);
*/
// SPREADING
// NumericMatrix sir_ml(const RMLNetwork& mnet, double beta, int tau, long num_iterations);

/*

// MEASURE DISTRIBUTIONS

NumericVector degree_dist(const MLNetwork& mnet, const CharacterVector& layers, const std::string& type);

NumericVector neighborhood_dist(const MLNetwork& mnet, const CharacterVector& layers, const std::string& type);
NumericVector xneighborhood_dist(const MLNetwork& mnet, const CharacterVector& layers, const std::string& type);
*/
#endif /* _R_FUNCTIONS_H_ */
