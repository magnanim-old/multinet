/*
 * transformation.h
 *
 * Functions to transform a set of layers into a single one.
 */

#ifndef MULTIPLENETWORK_TRANSFORMATION_H_
#define MULTIPLENETWORK_TRANSFORMATION_H_

#include "datastructures.h"
#include <unordered_set>

namespace mlnet {

/**
 * The "weighted" flattening approach adds an edge between v1 and v2 in the flattened network if
 * the edge is present in any on the input layers.
 * The weight on the edge indicates the number of layers where the edge is present.
 */
LayerSharedPtr flatten_weighted(MLNetworkSharedPtr mnet, const std::string& new_layer_name, const std::unordered_set<LayerSharedPtr>& layers, bool force_directed, bool force_actors);

LayerSharedPtr flatten_or(MLNetworkSharedPtr mnet, const std::string& new_layer_name, const std::unordered_set<LayerSharedPtr>& layers, bool force_directed, bool force_actors);

//LayerSharedPtr flatten_multi(MLNetworkSharedPtr mnet, const std::string& new_layer_name, const std::unordered_set<LayerSharedPtr>& layers, bool force_directed);

LayerSharedPtr project_clique(MLNetworkSharedPtr mnet, const std::string& new_layer_name, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);

// utility function used in multiple types of flattening
LayerSharedPtr create_layer(MLNetworkSharedPtr mnet, const std::string& new_layer_name, const std::unordered_set<LayerSharedPtr>& layers, bool force_directed, bool force_actors);

}

#endif /* MULTIPLENETWORK_TRANSFORMATION_H_ */
