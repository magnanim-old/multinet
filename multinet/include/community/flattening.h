#ifndef COMMUNITY_FLATTENING_H_
#define COMMUNITY_FLATTENING_H_

namespace mlnet {

enum WeighteningType {ZeroOne=0, NumOfLayers=1, Neighborhood=2};
enum SingleLayerAlgorithm {LabelPropagation =1};

CommunityStructureSharedPtr flattenAndDetectComs(const MLNetworkSharedPtr& mnet, WeighteningType wType,SingleLayerAlgorithm slAlgo);
}

#endif /* COMMUNITY_FLATTENING_H_ */
