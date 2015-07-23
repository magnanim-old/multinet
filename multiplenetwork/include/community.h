#ifndef MULTIPLENETWORK_COMMUNITY_H_
#define MULTIPLENETWORK_COMMUNITY_H_

#include <vector>
#include "datastructures.h"

// NOT YET CONSOLIDATED

namespace mlnet {

void girwan_newman(MLNetwork& mnet, std::map<layer_id,std::map<node_id,long> >& communities);

}
#endif /* MULTIPLENETWORK_COMMUNITY_H_ */

