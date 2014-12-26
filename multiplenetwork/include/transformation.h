/*
 * transformation.h
 *
 * Functions to transform a set of networks into a single one.
 */

#ifndef MULTIPLENETWORK_TRANSFORMATION_H_
#define MULTIPLENETWORK_TRANSFORMATION_H_

#include "datastructures.h"

/**
 * The "weighted" flattening approach adds an edge between v1 and v2 in the flattened network if
 * the edge is present in any on the input networks.
 * The weight on the edge indicates the number of networks where the edge is present.
 */
Network flatten_weighted(const MultiplexNetwork& mnet, const std::set<network_id>& networks, bool force_directed, bool force_all_actors);

Network flatten_weighted(const MultiplexNetwork& mnet, const std::set<std::string>& networks, bool force_directed, bool force_all_actors);

Network flatten_or(const MultiplexNetwork& mnet, const std::set<network_id>& networks, bool force_directed, bool force_all_actors);

Network flatten_or(const MultiplexNetwork& mnet, const std::set<std::string>& networks, bool force_directed, bool force_all_actors);

Network flatten_multi(const MultiplexNetwork& mnet, const std::set<network_id>& networks, bool force_directed, bool force_all_actors);

Network flatten_multi(const MultiplexNetwork& mnet, const std::set<std::string>& networks, bool force_directed, bool force_all_actors);

#endif /* MULTIPLENETWORK_TRANSFORMATION_H_ */
