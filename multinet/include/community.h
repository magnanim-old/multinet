#ifndef MULTINET_COMMUNITY_H_
#define MULTINET_COMMUNITY_H_

#include "datastructures.h"

namespace mlnet {

/* COMMON DATA STRUCTURES */

    class community;
    class actor_community;
    class community_structure;
    class actor_community_structure;
    typedef std::shared_ptr<community> CommunitySharedPtr;
    typedef std::shared_ptr<actor_community> ActorCommunitySharedPtr;
    typedef std::shared_ptr<community_structure> CommunityStructureSharedPtr;
    typedef std::shared_ptr<actor_community_structure> ActorCommunityStructureSharedPtr;
    enum EdgeBelonigngFunc {Sum =1,Multiply=2,Max=3,Average=4};
    /**
     * A community - that is a set of nodes.
     */
    class community {
        protected:
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
        void add_community(const CommunitySharedPtr&);
        CommunitySharedPtr get_community(int i);
        const vector<CommunitySharedPtr>& get_communities() const;

        void print(std::ostream& stream);

        private:
        vector<CommunitySharedPtr> data;
    };


    /**
     * A set of communities. Each community can be accessed by index.
     */
    class actor_community_structure {
    private:
        actor_community_structure();

    public:
        static ActorCommunityStructureSharedPtr create();
        std::string to_string() const;
        void add_community(const ActorCommunitySharedPtr&);
        ActorCommunitySharedPtr get_community(int i);
        const vector<ActorCommunitySharedPtr>& get_communities() const;

        void print(std::ostream& stream);

    private:
        vector<ActorCommunitySharedPtr> data;
    };


    /* Community translation functions */
    CommunityStructureSharedPtr to_node_communities(const ActorCommunityStructureSharedPtr& comm, const MLNetworkSharedPtr& net);
    CommunitySharedPtr to_node_community(const ActorCommunitySharedPtr& comm, const MLNetworkSharedPtr& net);

    /* Community evaluation (internal) */

    double modularity(const MLNetworkSharedPtr& mnet, const CommunityStructureSharedPtr& groups, double c);
    double modularity(const MLNetworkSharedPtr& mnet, const hash_map<NodeSharedPtr,long>& groups, double c); // for back-compatibility
    hash_map<CommunitySharedPtr,hash_map<NodeSharedPtr,double>> get_nodes_belonging_coef(const CommunityStructureSharedPtr& communities);
    double extended_modularity(const MLNetworkSharedPtr& mnet, const CommunityStructureSharedPtr& communities, hash_map<CommunitySharedPtr,hash_map<NodeSharedPtr,double>> nodes_belonging_coefficients,EdgeBelonigngFunc func);
    /* Community comparison functions (external) */

    double community_jaccard(const CommunitySharedPtr& c1, const CommunitySharedPtr& c2);

    double purity(const CommunityStructureSharedPtr& com1, const CommunityStructureSharedPtr& com2);

    double rand_index(const CommunityStructureSharedPtr& com1, const CommunityStructureSharedPtr& com2);

    double normalized_mutual_information(const CommunityStructureSharedPtr& com1, const CommunityStructureSharedPtr& com2, int n);

    double omega_index(const CommunityStructureSharedPtr& partitioning1, const CommunityStructureSharedPtr& partitioning2,const MLNetworkSharedPtr& mnet);

    double f_measure(const CommunityStructureSharedPtr& com1, const CommunityStructureSharedPtr& com2);

    double fm_index(const CommunityStructureSharedPtr& com1, const CommunityStructureSharedPtr& com2);

    CommunityStructureSharedPtr read_ground_truth(const string& infile, char separator, const MLNetworkSharedPtr& mnet);

    /* ALGORITHMS */


}

#include "community/cutils.h"
#include "community/labelpropagationsinglelayer.h"
#include "community/flattening.h"
#include "community/acl.h"
#include "community/ml-cpm.h"
#include "community/lart.h"
#include "community/pmm.h"
#include "community/mlp.h"
#include "community/glouvain.h"
#include "community/abacus.h"

#endif /* MULTINET_COMMUNITY_H_ */
