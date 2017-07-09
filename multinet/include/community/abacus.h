#ifndef ABACUS_H_
#define ABACUS_H_

namespace mlnet {
    
    /*  */
    ActorCommunityStructureSharedPtr eclat_merge(const MLNetworkSharedPtr& mnet, const hash_map<LayerSharedPtr,CommunityStructureSharedPtr>& single_layer_communities, int min_actors, int min_layers);
    
    /**
     * Finds communities using the abacus algorithm, itself using a label propagation
     * single-layer clustering algorithm as in the original paper.
     * @param mnet input multilayer network
     * @param min_actors minimum number of actors in a community
     * @param min_layers minimum number of layers in a community
     * @return a set of actor communities
     */
    ActorCommunityStructureSharedPtr abacus(const MLNetworkSharedPtr& mnet, int min_actors, int min_layers);

}

#endif /* ABACUS_H_ */
