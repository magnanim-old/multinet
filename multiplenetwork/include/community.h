/**
 * creation.h
 * Functions to read multiple networks from files
 *
 * @version 1.0 (Jun 12, 2013)
 * @author: Matteo Magnani <matteo.magnani@it.uu.se>
 */

#ifndef MULTIPLENETWORK_COMMUNITY_H_
#define MULTIPLENETWORK_COMMUNITY_H_

#include <vector>
#include "datastructures.h"

void girwan_newman(MultilayerNetwork& mnet, std::map<network_id,std::map<vertex_id,long> >& communities);

#endif /* MULTIPLENETWORK_COMMUNITY_H_ */

