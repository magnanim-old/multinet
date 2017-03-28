#ifndef GLOUVAIN_H_
#define GLOUVAIN_H_

#include "cutils.h"

namespace mlnet {

class glouvain {

public:

	/*
		Use : glouvain g;
			 g.get_ml_community(MLNetworkSharedPtr);
	*/
	hash_set<ActorSharedPtr> get_ml_community(MLNetworkSharedPtr mnet, double gamma, double omega);

};


}

#endif
