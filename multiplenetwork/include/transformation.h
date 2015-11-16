/*
 * transformation.h
 *
 * Functions to transform a set of layers into a single one.
 */

#ifndef MULTIPLENETWORK_TRANSFORMATION_H_
#define MULTIPLENETWORK_TRANSFORMATION_H_

#include "datastructures.h"

namespace mlnet {

/**
 * The "weighted" flattening approach adds an edge between v1 and v2 in the flattened network if
 * the edge is present in any on the input networks.
 * The weight on the edge indicates the number of networks where the edge is present.
 */
void flatten_weighted(MLNetworkSharedPtr mnet, const std::string& new_layer_name, const std::unordered_set<LayerSharedPtr>& layers, bool force_directed);

void flatten_or(MLNetworkSharedPtr mnet, const std::string& new_layer_name, const std::unordered_set<LayerSharedPtr>& layers, bool force_directed);

void flatten_multi(MLNetworkSharedPtr mnet, const std::string& new_layer_name, const std::unordered_set<LayerSharedPtr>& layers, bool force_directed);

void project(MLNetworkSharedPtr mnet, const std::string& new_layer_name, const std::unordered_set<LayerSharedPtr>& layers, bool force_directed);

}

#endif /* MULTIPLENETWORK_TRANSFORMATION_H_ */
