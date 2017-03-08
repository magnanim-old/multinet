#ifndef MULTIPLENETWORK_COMMUNITY_H_
#define MULTIPLENETWORK_COMMUNITY_H_

#include "datastructures.h"

namespace mlnet {

    
/* COMMON DATA STRUCTURES */

    class community {
        public:
        community();
        std::string to_string() const;
        void add_node(NodeSharedPtr);
        const hash_set<NodeSharedPtr>& get_nodes() const;
        
        private:
        hash_set<NodeSharedPtr> data;
    };
    
    typedef std::shared_ptr<community> CommunitySharedPtr;
    
    class communities {
        public:
        communities();
        std::string to_string() const;
        void add_community(CommunitySharedPtr);
        const hash_set<CommunitySharedPtr>& get_communities() const;
        
        private:
        hash_set<CommunitySharedPtr> data;
    };
    
    typedef std::shared_ptr<communities> CommunitiesSharedPtr;

/* ALGORITHMS */

    
}

#endif /* MULTIPLENETWORK_COMMUNITY_H_ */

