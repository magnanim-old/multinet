#ifndef MULTINET_COMMUNITY_H_
#define MULTINET_COMMUNITY_H_

#include "datastructures.h"

namespace mlnet {

/* COMMON DATA STRUCTURES */

    class community;
    class communities;
    typedef std::shared_ptr<community> CommunitySharedPtr;
    typedef std::shared_ptr<communities> CommunitiesSharedPtr;

    /**
     * A community - that is a set of nodes.
     */
    class community {
        private:
        community();

        public:
        static CommunitySharedPtr create();
        std::string to_string() const;
        void add_node(NodeSharedPtr);
        const hash_set<NodeSharedPtr>& get_nodes() const;

        private:
        hash_set<NodeSharedPtr> data;
    };

        /**
     * A set of communities. Each community can be accessed by index.
     */
    class communities {
        private:
        communities();

        public:
        static CommunitiesSharedPtr create();
        std::string to_string() const;
        void add_community(CommunitySharedPtr);

        CommunitySharedPtr get_community(int i);
        const vector<CommunitySharedPtr>& get_communities() const;

        private:
        vector<CommunitySharedPtr> data;
    };



}

#endif /* MULTINET_COMMUNITY_H_ */
