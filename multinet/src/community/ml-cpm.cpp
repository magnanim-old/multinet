/*
 * ML-Clique percolation method
 *
 */

#include "utils.h"
#include "community.h"

namespace mlnet {
    

    // ALGORITHM
    
    CommunityStructureSharedPtr mlcpm(const MLNetworkSharedPtr& mnet, size_t k, size_t m) {
        // Step 1: find max-cliques
        hash_set<CliqueSharedPtr> cliques = find_max_cliques(mnet,k,m);
        if (cliques.size()==0)
            return community_structure::create();
        // Step 2: bluid adjacency graph
        std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> > adjacency = build_max_adjacency_graph(cliques,k,m); // TODO create a simple_graph data structure
        // Step 3: extract communities
        hash_set<AdjCliqueCommunitySharedPtr> comm = find_max_communities(mnet,adjacency,m);
        // Translate the result, so that it is compatible with the other clustering algorithms
        CommunityStructureSharedPtr result = community_structure::create();
        for (AdjCliqueCommunitySharedPtr c: comm) {
            result->add_community(c->to_community(mnet));
        }
        return result;
    }
    
    // STEP 1: FIND MAX CLIQUES
    hash_set<CliqueSharedPtr> find_max_cliques(const MLNetworkSharedPtr& mnet, size_t k, size_t m) {
        
        hash_set<CliqueSharedPtr> result;
        
        // We first define the stack. "instance" is an element on the stack, with information about the clique we are currently processing.
        struct instance {
            // skip is an optimization indicating how many elements can be removed from the stack without changing the final results. Setting skip=1 is the same as not using this.
            int skip;
            // This is the current clique being processed. When the processing is done and the current instance is popped out of the stack, all maximal cliques including A must have been returned.
            CliqueSharedPtr A;
            // This is the list of nodes that can be used to extend A, so that A_ext is still a clique.
            std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > > B;
            // This is the list of nodes that can be used to extend A, so that A_ext is still a clique, but that can have already been processed. This list is kept to avoid that the same clique is computed following different orders, e.g., (n1,n2,n3) and (n1,n3,n2)
            std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > > C;
            // constructor
            instance(int skip,
                     CliqueSharedPtr& A,
                     std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > >& B,
                     std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > >& C)
            : skip(skip), A(A), B(B), C(C) {}
        };
        vector<std::shared_ptr<instance> > stack;
        
        // We now initialize the stack with an empty clique, that the function will try to extend with nodes in B (containing all the nodes)
        // A: empty clique
        CliqueSharedPtr A = clique::create(); //TODO add create method
        for (LayerSharedPtr layer: *mnet->get_layers())
            A->layers.insert(layer);
        // B: all actors. For each actor we keep the set of layers where it is present
        std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > > B;
        for (ActorSharedPtr actor: *mnet->get_actors()) {
            std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > b(actor,hash_set<LayerSharedPtr>());
            for (NodeSharedPtr node: *mnet->get_nodes(actor)) {
                b.second.insert(node->layer);
            }
            B.push_back(b);
        }
        // C: empty set of actors
        std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > > C;
        
        
        // PROCESSING STARTS HERE!
        stack.push_back(std::shared_ptr<instance>(new instance(1,A,B,C)));
        
        while (!stack.empty()) {
            
            // fetch clique to be processed
            std::shared_ptr<instance> inst = stack.back();
            
            // If no elements are left to extend the clique: remove from stack
            if (inst->B.size()==0) {
                for (int i=0; i< inst->skip; i++) stack.pop_back();
                continue;
            }
            
            // Extend the current clique with one of the elements from B
            auto b = inst->B.begin();
            
            // new clique: A_ext
            hash_set<LayerSharedPtr> new_layers = s_intersection(inst->A->layers,b->second);
            hash_set<ActorSharedPtr> new_actors;
            for (auto actor: inst->A->actors)
                new_actors.insert(actor);
            new_actors.insert(b->first);
            CliqueSharedPtr A_ext = clique::create(new_actors,new_layers);
            
            // new set of actors that can be used to extend the clique at the next iteration: B_ext
            bool reset_skip = false;
            std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > > B_ext;
            auto q = b;
            for (++q; q!=inst->B.end(); ++q) {
                hash_set<LayerSharedPtr> common = s_intersection(new_layers,neighboring_layers(mnet,b->first,q->first)); //TODO encapsulate these two intersections into a single function computing them together, for increased readability...
                if (common.size()>=m) {
                    std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > new_b(q->first,hash_set<LayerSharedPtr>(common.begin(),common.end()));
                    B_ext.push_back(new_b);
                }
                // not all layers have been used: we will have to process this again
                if (common.size()<q->second.size()) reset_skip=true;
            }
            
            // new set of actors that can be used to extend the clique at the next iteration and have been already used before: C_ext
            std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > > C_ext;
            for (auto c = inst->C.begin(); c!=inst->C.end(); ++c) {
                hash_set<LayerSharedPtr> common = s_intersection(new_layers,neighboring_layers(mnet,b->first,c->first));
                if (common.size()>=m) {
                    std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > new_c(c->first,hash_set<LayerSharedPtr>(common.begin(),common.end()));
                    C_ext.push_back(new_c);
                }
            }
            
            // move b from B to C
            inst->C.push_back(*b);
            inst->B.erase(b);
            
            // check if A_ext can lead to new results, or if it is a new result (that is, maximal) itself
            
            // impossible to grow the current clique to have at least k actors: do not stack it.
            if (A_ext->actors.size()+B_ext.size()<k) {
                continue;
            }
            
            // the current clique has less than m layers: do not stack it.
            if (A_ext->layers.size()<m) {
                continue;
            }
            
            // Check if A_ext should be returned: if
            // (1) it has at least k actors
            // (2) it cannot be extended by any new actor maintaining the current number of layers
            // (3) it cannot be extended by an already processed actors maintaining the current number of layers (otherwise, this sub-clique has already been processed previously by the algorithm)
            //TODO: only max_layers, or is it the intersection that matters? For efficiency reasons.
            
            bool can_extend_on_B = false;
            bool can_extend_on_C = false;
            
            size_t max_layers_C = A_ext->layers.size();
            for (auto c: C_ext) {
                size_t common_layers = s_intersection(A_ext->layers,c.second).size();
                if (common_layers==max_layers_C) {
                    can_extend_on_C=true;
                    continue;
                }
            }
            size_t max_layers_B = inst->A->layers.size();
            for (auto b: B_ext) {
                size_t common_layers = s_intersection(A_ext->layers,b.second).size();
                if (common_layers==max_layers_B) {
                    can_extend_on_B=true;
                    continue;
                }
            }
            if (A_ext->actors.size()>=k &&
                !can_extend_on_B &&
                !can_extend_on_C) {
                result.insert(A_ext);
            }
            
            // put the new clique under processing
            stack.push_back(std::shared_ptr<instance>(new instance(reset_skip?1:inst->skip+1,A_ext,B_ext,C_ext)));
            
        }
        return result;
    }
    
    
    hash_set<LayerSharedPtr> neighboring_layers(MLNetworkSharedPtr mnet, ActorSharedPtr actor1, ActorSharedPtr actor2) {
        hash_set<LayerSharedPtr> result;
        for (NodeSharedPtr node: *mnet->get_nodes(actor1)) {
            NodeSharedPtr node2 = mnet->get_node(actor2,node->layer);
            if (!node2) continue;
            if (mnet->get_edge(node,node2)) {
                result.insert(node->layer);
            }
        }
        return result;
    }
    
    
    std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> > build_max_adjacency_graph(const hash_set<CliqueSharedPtr>& C, size_t k, size_t m) {
        std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> > result;
        for (CliqueSharedPtr c1: C) {
            result[c1];
            for (CliqueSharedPtr c2: C) {
                if (c1<=c2)
                    continue;
                size_t common_actors = s_intersection(c1->actors,c2->actors).size();
                size_t common_layers = s_intersection(c1->layers,c2->layers).size();
                if (common_actors>=k-1 && common_layers>=m) {
                    result[c1].insert(c2);
                    result[c2].insert(c1);
                }
            }
        }
        return result;
    }
    
    hash_set<AdjCliqueCommunitySharedPtr> find_max_communities(MLNetworkSharedPtr mnet, const std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> >& adjacency, size_t m) {
        // result: empty set of communities
        hash_set<AdjCliqueCommunitySharedPtr> result;
        // A: empty community on all layers
        AdjCliqueCommunitySharedPtr A = cpm_community::create();
        for (LayerSharedPtr layer: *mnet->get_layers()) {
            A->add_layer(layer);
        }
        
        hash_set<CliqueSharedPtr> AlreadySeen;
        for (auto clique_pair : adjacency) {
            AdjCliqueCommunitySharedPtr A = cpm_community::create();
            A->add_clique(clique_pair.first);
            for (LayerSharedPtr layer: clique_pair.first->layers)
                A->add_layer(layer);
            vector<CliqueSharedPtr> Candidates(clique_pair.second.begin(),clique_pair.second.end());
            layer_sets empty;
            find_max_communities(adjacency,A,Candidates,AlreadySeen,empty,m,result);
            AlreadySeen.insert(clique_pair.first);
        }
        return result;
    }
    
    void find_max_communities(const std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> >& adjacency, AdjCliqueCommunitySharedPtr& A,
                              vector<CliqueSharedPtr> Candidates, hash_set<CliqueSharedPtr>& processedCliques, layer_sets& processedLayerCombinations, size_t m, hash_set<AdjCliqueCommunitySharedPtr>& result) {
        
        vector<CliqueSharedPtr> stack;
        
        // EXPAND
        while (Candidates.size()!=0) {
            CliqueSharedPtr c = Candidates.back();
            Candidates.pop_back();
            hash_set<LayerSharedPtr> i = s_intersection(A->get_layers(),c->layers);
            if (i.size()==A->get_layers().size()) {
                if (processedCliques.count(c)>0) {
                    return;
                }
                A->cliques.insert(c);
                
                for (auto j: adjacency.at(c)) {
                    if (A->cliques.count(j)>0)
                        continue;
                    else {
                        Candidates.push_back(j);
                    }
                }
            }
            else if (i.size()>=m) {
                if (processedLayerCombinations.count(c->layers)==0) {
                    stack.push_back(c);
                }
            }
        }
        result.insert(A);
        processedLayerCombinations.insert(A->get_layers());
        
        layer_sets candidate_layer_combinations;
        for (CliqueSharedPtr c: stack) {
            hash_set<LayerSharedPtr> s = s_intersection(A->get_layers(),c->layers);
            sorted_set<LayerSharedPtr> to_be_processed(s.begin(), s.end()); // FIXME
            candidate_layer_combinations.insert(to_be_processed);
        }
        for (sorted_set<LayerSharedPtr> layers: candidate_layer_combinations) {
            AdjCliqueCommunitySharedPtr comm = cpm_community::create();
            comm->cliques.insert(A->cliques.begin(),A->cliques.end());
            comm->layers.insert(layers.begin(),layers.end());
            find_max_communities(adjacency,comm,stack,processedCliques,processedLayerCombinations,m,result);
        }
    }
    
    
    // DATA STRUCTURES
    
    cpm_community::cpm_community() :
    id(0) {}
    
    cpm_community::cpm_community(long id, hash_set<CliqueSharedPtr> cliques, hash_set<LayerSharedPtr> layers) :
    id(id), cliques(cliques.begin(),cliques.end()), layers(layers.begin(),layers.end()) {}
    
    std::set<ActorSharedPtr> cpm_community::actors() const {
        std::set<ActorSharedPtr> actors;
        for (CliqueSharedPtr clique: cliques) {
            for (ActorSharedPtr actor: clique->actors)
                actors.insert(actor);
        }
        return actors;
    }
    
    AdjCliqueCommunitySharedPtr cpm_community::create() {
        return AdjCliqueCommunitySharedPtr(new cpm_community());
    }
    
    void cpm_community::add_clique(CliqueSharedPtr clique) {
        cliques.insert(clique);
    }
    
    void cpm_community::add_layer(LayerSharedPtr layer) {
        layers.insert(layer);
    }
    
    const std::set<LayerSharedPtr>& cpm_community::get_layers() {
        return layers;
    }
    
    CommunitySharedPtr cpm_community::to_community(const MLNetworkSharedPtr& net) const {
        CommunitySharedPtr result = community::create();
        for (ActorSharedPtr actor: actors()) {
            for (LayerSharedPtr layer: layers) {
                NodeSharedPtr n = net->get_node(actor,layer);
                if (n!=NULL) result->add_node(n);
            }
        }
        return result;
    }
    
    std::string cpm_community::to_string() {
        std::string res = "C" + std::to_string(id) + ": ";
        hash_set<ActorSharedPtr> actors;
        for (CliqueSharedPtr clique: cliques) {
            for (ActorSharedPtr actor: clique->actors)
                actors.insert(actor);
        }
        for (ActorSharedPtr actor: actors)
            res += actor->name + " ";
        res += "( ";
        for (LayerSharedPtr layer: layers) {
            res += layer->name + " ";
        }
        res += ")";
        return res;
    }
    
    int cpm_community::size() const {
        hash_set<ActorSharedPtr> actors;
        for (CliqueSharedPtr clique: cliques) {
            for (ActorSharedPtr actor: clique->actors)
                actors.insert(actor);
        }
        return actors.size();
    }
    
}
