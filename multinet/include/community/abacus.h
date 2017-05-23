#ifndef ABACUS_H_
#define ABACUS_H_

namespace mlnet {

    CommunityStructureSharedPtr abacus(const MLNetworkSharedPtr& mnet, const vector<CommunityStructureSharedPtr>& single_layer_communities, int min_support);

}

#endif /* ABACUS_H_ */
