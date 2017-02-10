#ifndef LBL_H_
#define LBL_H_

#include "datastructures.h"

namespace mlnet {

class abacus {

	public:
		/*
			Use: abacus k;
				 hash_set<CommunitySharedPtr> h = k.get_ml_community(MLNetworkSharedPtr, SLCDAlgorithm, float);
			Pre: MLNetworkSharedPtr is a multilayer network
				sigma is the support threshold for association rule mining
				SLCDAlgorithm is a single layer community detection algorithm
			Post: h is a hash set containing actor communities
					e.x: print hash_set[A]
						>>> B,C,D
		*/
		hash_set<CommunitySharedPtr> get_ml_community(MLNetworkSharedPtr mnet, float sigma, SLCDAlgorithm alg, Args... args);
};

class pmm {

	public:
		/*
			Use: pmm k;
				 hash_set<CommunitySharedPtr> h = k.get_ml_community(MLNetworkSharedPtr, SLCDAlgorithm, float);
			Pre: MLNetworkSharedPtr is a multilayer network
				k is number of communities to extract
				ell is the number of structural features to extract from each dimension
				maxKmeans is the number of kmeans runs to repeat
			Post: h is a hash set containing actor communities
					e.x: print hash_set[A]
						>>> B,C,D
		*/
		hash_set<CommunitySharedPtr> get_ml_community(MLNetworkSharedPtr mnet, uint32_t k, uint32_t ell, uint32_t maxKmeans);


};

#endif
