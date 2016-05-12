/*
 * transformation.h
 *
 * Functions to transform a set of layers into a single one. A flattening is used when the same actors
 * are present on multiple layers: a new, single layer is created, and edges from all the flattened layers
 * contribute to the connectivity of the new layer. A projection creates a new layer where actors are taken from one layer and their connections depend
 * on how they are connected to objects in a second layer. Different types of flattening and projection
 * exist.
 */

#ifndef MLNET_TRANSFORMATION_H_
#define MLNET_TRANSFORMATION_H_

#include "datastructures.h"
#include <unordered_set>

namespace mlnet {

/**
 * The "weighted" flattening approach adds an edge between a1 and a2 in the flattened network,
 * stored as a new layer, if the edge is present in any on the input layers, or combinations of them.
 * The weight on the edge indicates the number of layers where the edge is present.
 * @param mnet The multilayer network containing the layers to be merged.
 * @param new_layer_name The name of a new layer, added to the input multilayer network and obtained as a flattening of some of its layers.
 * @param layers The set of layers to be flattened.
 * @param force_directed If true, the flattened layer will contain directed edges. If false, it will contain directed edges only if at least one of the flattened layers do.
 * @param force_actors If true, all the actors in the multilayer network will be included in the flattened layer, even if they do not appear in any of the input layers.
 * @return A pointer to the newly created layer.
 * @throws DuplicateElementException If a layer with the same name already exists.
 */
LayerSharedPtr flatten_weighted(MLNetworkSharedPtr& mnet, const std::string& new_layer_name, const simple_set<LayerSharedPtr>& layers, bool force_directed, bool force_actors);

/**
 * The "disjunctive" flattening approach, also known as or-flattening and unweighted-fattening,
 * adds an edge between a1 and a2 in the flattened network, stored as a new layer, if the edge
 * is present in any on the input layers, or combinations of them. There is no difference
 * in the result if an edge is present one or more times in the input layers.
 * @param mnet The multilayer network containing the layers to be merged.
 * @param new_layer_name The name of a new layer, added to the input multilayer network and obtained as a flattening of some of its layers.
 * @param layers The set of layers to be flattened.
 * @param force_directed If true, the flattened layer will contain directed edges. If false, it will contain directed edges only if at least one of the flattened layers do.
 * @param force_actors If true, all the actors in the multilayer network will be included in the flattened layer, even if they do not appear in any of the input layers.
 * @return A pointer to the newly created layer.
 * @throws DuplicateElementException If a layer with the same name already exists.
 */
LayerSharedPtr flatten_unweighted(MLNetworkSharedPtr& mnet, const std::string& new_layer_name, const std::unordered_set<LayerSharedPtr>& layers, bool force_directed, bool force_actors);

/**
 * A projection creates a new layer where actors are taken from one layer and their connections depend
 * on how they are connected to objects in a second layer. An unweighted projection adds an edge between
 * a1 and a2 (from layer 1) in the new layer if a1 and a2 are both connected to the same object in layer 2.
 * We say that layer 2 is projected into layer 1.
 * @param mnet The multilayer network containing the layers to be merged.
 * @param new_layer_name The name of a new layer, added to the input multilayer network and obtained as a projection of layer 2 into layer 1.
 * @param layer1 The layer containing the actors who will populate the projection.
 * @param layer2 The layer projected into layer 1.
 * @return A pointer to the newly created layer.
 * @throws DuplicateElementException If a layer with the same name already exists.
 */
LayerSharedPtr project_unweighted(MLNetworkSharedPtr& mnet, const std::string& new_layer_name, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);

/**
 * Adds a new layer to a multilayer network. This is an utility function used inside different types of flattening and projection.
 * @param mnet A multilayer network.
 * @param new_layer_name The name of a new layer, added to the input multilayer network.
 * @param layers The set of layers determining the directionality of the new layer.
 * @return A pointer to the newly created layer.
 * @throws DuplicateElementException If a layer with the same name already exists.
 */
LayerSharedPtr create_layer(MLNetworkSharedPtr& mnet, const std::string& new_layer_name, const simple_set<LayerSharedPtr>& layers, bool force_directed, bool force_actors);

}

#endif /* MLNET_TRANSFORMATION_H_ */
