#ifndef MULTINET_COMMUNITY_H_
#define MULTINET_COMMUNITY_H_

#include "datastructures.h"

namespace mlnet {

    
/* COMMON DATA STRUCTURES */
    
    class community;
    class actor_community;
    class community_structure;
    typedef std::shared_ptr<community> CommunitySharedPtr;
    typedef std::shared_ptr<actor_community> ActorCommunitySharedPtr;
    typedef std::shared_ptr<community_structure> CommunityStructureSharedPtr;

    /**
     * A community - that is a set of nodes.
     */
    class community {
        protected:
        community();
        
        public:
        static CommunitySharedPtr create();
        std::string to_string() const;
        void add_node(NodeSharedPtr);
        const hash_set<NodeSharedPtr>& get_nodes() const;
        int num_nodes() const;
        
        private:
        hash_set<NodeSharedPtr> data;
    };
    
    /**
     * A community - that is a set of actors on a set of layers.
     */
    class actor_community {
    private:
        actor_community();
        
    public:
        static ActorCommunitySharedPtr create();
        CommunitySharedPtr to_community();
        std::string to_string() const;
        void add_actor(ActorSharedPtr);
        const hash_set<ActorSharedPtr>& get_actors() const;
        int num_actors() const;
        void add_layer(LayerSharedPtr);
        const hash_set<LayerSharedPtr>& get_layers() const;
        int num_layers() const;
        
    private:
        hash_set<ActorSharedPtr> actors;
        hash_set<LayerSharedPtr> layers;
    };
    
    /**
     * A set of communities. Each community can be accessed by index.
     */
    class community_structure {
        private:
        community_structure();
        
        public:
        static CommunityStructureSharedPtr create();
        std::string to_string() const;
        void add_community(CommunitySharedPtr);
        CommunitySharedPtr get_community(int i);
        const vector<CommunitySharedPtr>& get_communities() const;
        
        private:
        vector<CommunitySharedPtr> data;
    };
}


/* ALGORITHMS */
#include "community/ml-cpm.h"

#endif /* MULTINET_COMMUNITY_H_ */

