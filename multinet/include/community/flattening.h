#ifndef FLATTENING_H_
#define FLATTENING_H_

#include "datastructures.h"

namespace mlnet {

class berlingerio {

	public:

		/*
			Use: berlingerio k;
				 hash_set<CommunitySharedPtr> h = k.get_ml_community(MLNetworkSharedPtr);
			Pre: MLNetworkSharedPtr is a multilayer network
			Post: h is a hash set ccontaining actor communities
					e.x: hash_set[A] = B,C,D
		*/
		hash_set<CommunitySharedPtr> get_ml_community(MLNetworkSharedPtr mnet);

};

#endif

