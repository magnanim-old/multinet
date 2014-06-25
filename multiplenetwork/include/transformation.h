/*
 * transformation.h
 *
 *  Created on: Jun 11, 2013
 *      Author: magnanim
 */

#ifndef MULTIPLENETWORK_TRANSFORMATION_H_
#define MULTIPLENETWORK_TRANSFORMATION_H_

#include "datastructures.h"

Network flatten_weighted(const MultiplexNetwork& mnet, const std::set<network_id>& active_networks);

Network flatten_weighted(const MultiplexNetwork& mnet, const std::set<std::string>& active_networks);

#endif /* MULTIPLENETWORK_TRANSFORMATION_H_ */
