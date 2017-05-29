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
        private:
        community();

        public:
        static CommunitySharedPtr create();
        std::string to_string() const;
        void add_node(const NodeSharedPtr&);
        const hash_set<NodeSharedPtr>& get_nodes() const;

        private:
        hash_set<NodeSharedPtr> data;
    };

    /**
     * A community, defined as a set of actors in a set of layers.
     */
    class actor_community : public community {
    private:
        actor_community();

    public:
        static ActorCommunitySharedPtr create();
        std::string to_string() const;
        void add_node(const NodeSharedPtr&);
        const hash_set<NodeSharedPtr>& get_nodes() const;
        void add_actor(const ActorSharedPtr&);
        const hash_set<ActorSharedPtr>& get_actors() const;
        void add_layer(const LayerSharedPtr&);
        const hash_set<LayerSharedPtr>& get_layers() const;

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
        void add_community(const CommunitySharedPtr&);
        CommunitySharedPtr get_community(int i);
        const vector<CommunitySharedPtr>& get_communities() const;

        void print(std::ostream& stream);

        private:
        vector<CommunitySharedPtr> data;
    };

    /* Community comparison functions */

    double community_jaccard(const CommunitySharedPtr& c1, const CommunitySharedPtr& c2);

    double purity(const CommunityStructureSharedPtr& com1, const CommunityStructureSharedPtr& com2);

    double rand_index(const CommunityStructureSharedPtr& com1, const CommunityStructureSharedPtr& com2);

    double normalized_mutual_information(const CommunityStructureSharedPtr& com1, const CommunityStructureSharedPtr& com2, int n);

    double f_measure(const CommunityStructureSharedPtr& com1, const CommunityStructureSharedPtr& com2);

    double fm_index(const CommunityStructureSharedPtr& com1, const CommunityStructureSharedPtr& com2);


    /* ALGORITHMS */

}

#endif /* MULTINET_COMMUNITY_H_ */
