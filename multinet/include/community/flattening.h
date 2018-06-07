#ifndef COMMUNITY_FLATTENING_H_
#define COMMUNITY_FLATTENING_H_

namespace mlnet {

enum WeighteningType {ZeroOne=0, NumOfLayers=1, Neighborhood=2, Jaccard=3};


MLNetworkSharedPtr  flatten(const MLNetworkSharedPtr& mnet, WeighteningType wType);


}

#endif /* COMMUNITY_FLATTENING_H_ */
