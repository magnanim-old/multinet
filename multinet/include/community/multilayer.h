#ifndef MULTILAYER_H_
#define MULTILAYER_H_

#include "datastructures.h"

namespace mlnet {

class lart {

	public:
		/*
			Use: lart k;
				 k.get_ml_community(MLNetworkSharedPtr, uint32_t, float, float);
			Pre: MLNetworkSharedPtr is a multilayer network
				 t is the number of number of steps for random walker to take
				 eps
				 gamma
		*/
		hash_set<CommunitySharedPtr> get_ml_community(MLNetworkSharedPtr mnet, uint32_t t, float eps, float gamma);

};

#endif

