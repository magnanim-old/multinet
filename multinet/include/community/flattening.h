#ifndef FLATTENING_H_
#define FLATTENING_H_

#include "community.h"

namespace mlnet {


enum WeighteningType {ZeroOne=0, NumOfLayers=1, Neighborhood=2};
enum SingleLayerAlgorithm {LabelPropagation =1};

ActorCommunityStructureSharedPtr  flattenAndDetectComs(const MLNetworkSharedPtr& mnet, WeighteningType wType,SingleLayerAlgorithm slAlgo);

}

#endif /* FLATTENING_H_ */
