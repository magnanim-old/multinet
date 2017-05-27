#ifndef ABACUS_H_
#define ABACUS_H_

namespace mlnet {
    
    ActorCommunityStructureSharedPtr eclat_merge(const MLNetworkSharedPtr& mnet, const hash_map<LayerSharedPtr,CommunityStructureSharedPtr>& single_layer_communities, int min_actors, int min_layers);
    
    ActorCommunityStructureSharedPtr abacus(const MLNetworkSharedPtr& mnet, int min_actors, int min_layers);

}

#endif /* ABACUS_H_ */
