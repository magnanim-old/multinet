/*
 * ML-Clique percolation method
 *
 */

#include "utils.h"

#include "community.h"

namespace mlnet {
    
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

// ALGORITHM

    CommunityStructureSharedPtr mlcpm(const MLNetworkSharedPtr& mnet, size_t k, size_t m) {
        // Step 1: find max-cliques
        hash_set<CliqueSharedPtr> C = find_max_cliques(mnet,k,m);
        //if (C.size()==0)
            return community_structure::create();
        // Step 2: bluid adjacency graph
        std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> > adjacency = build_max_adjacency_graph(C,k,m);
        // Step 3: extract communities
        hash_set<AdjCliqueCommunitySharedPtr> comm = find_max_communities_max_layers(mnet,adjacency,m);
        CommunityStructureSharedPtr result = community_structure::create();
        for (AdjCliqueCommunitySharedPtr c: comm) {
            result->add_community(c->to_community(mnet));
        }
        return result;
    }
    
    hash_set<CliqueSharedPtr> find_max_cliques(const MLNetworkSharedPtr& mnet, size_t k, size_t m) {
        
        hash_set<CliqueSharedPtr> result;
        
        // We first create the stack. "instance" is an element on the stack, with information about the clique we are currently processing.
        struct instance {
            int skip;
            // "status" is used to determine whether this is the first time we process this instance (status=0)
            int status;
            CliqueSharedPtr A;
            std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > > B;
            std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > > C;
            //hash_set<ActorSharedPtr> P; // what is P? :)
            instance(int skip,
                     int status,
                     CliqueSharedPtr& A,
                     std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > >& B,
                     std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > >& C)
            : skip(skip), status(status), A(A), B(B), C(C) {}
        };
        
        vector<std::shared_ptr<instance> > stack;
        
        // A: empty clique, that the function will try to extend with nodes in B
        CliqueSharedPtr A(new clique()); //TODO add create method
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
        
        stack.push_back(std::shared_ptr<instance>(new instance(1,0,A,B,C)));
        
        while (!stack.empty()) {
            
            //std::cout << "---------------------" << std::endl;
            std::shared_ptr<instance> inst = stack.back();
            
            // executed only the first time this instance is processed
            if (inst->status==0) {
                // impossible to grow the current clique to have at least k actors. Remove from stack.
                if (inst->A->actors.size()+inst->B.size()<k) {
                    //std::cout << "STOP: less than K actors available" << std::endl;
                    for (int i=0; i< inst->skip; i++) stack.pop_back();
                    //std::cout << "POP " << inst->skip << std::endl;
                    continue;
                }
                
                // the current clique has less than m layers. Remove from stack.
                if (inst->A->layers.size()<m) {
                    //std::cout << "STOP: less than M layers" << std::endl;
                    for (int i=0; i< inst->skip; i++) stack.pop_back();
                    //std::cout << "POP " << inst->skip << std::endl;
                    continue;
                }
                
                // Compute the ....... maximum number of layers for elements in B and C
                bool can_extend_on_B = false;
                bool can_extend_on_C = false;
                
                size_t max_layers_C = inst->A->layers.size();
                for (auto c: inst->C) {
                    size_t common_layers = s_intersection(inst->A->layers,c.second).size();
                    if (common_layers==max_layers_C) {
                        can_extend_on_C=true;
                        continue;
                    }
                }
                size_t max_layers_B = inst->A->layers.size();
                for (auto b: inst->B) {
                    size_t common_layers = s_intersection(inst->A->layers,b.second).size();
                    if (common_layers==max_layers_B) {
                        can_extend_on_B=true;
                        continue;
                    }
                }
                // return the current clique if:
                // (1) it has at least k actors
                // (2) it cannot be extended by any new actor maintaining the current number of layers
                // (3) it cannot be extended by an already processed actors maintaining the current number of layers (otherwise, this sub-clique has already been processed previously by the algorithm)
                //TODO: only max_layers, or is it the intersection that matters? For efficiency reasons.
                if (inst->A->actors.size()>=k &&
                    !can_extend_on_B &&
                    !can_extend_on_C) {
                    //A->id = result.size();
                    result.insert(inst->A);
                    //for (int i=0; i<A->actors.size(); i++) std::cout << " - ";
                    //std::cout << "RETURN " << inst->A->to_string() << std::endl;
                    // TMP
                    //inst->A->id = result.size();
                }
                // ONLY FOR DEBUG:
                //if (!(inst->A->actors.size()>=k)) std::cout << "NOT ENOUGH ACTORS / " << std::endl;
                //if (can_extend_on_B) std::cout << "CAN STILL BE EXTENDED ON THE SAME LAYERS / " << std::endl;
                //if (can_extend_on_C) std::cout << "PART OF AN ALREADY RETURNED CLIQUE" << std::endl;
                
                
                // we keep processing this, because it can still grow on at least m layers
                //inst->P.insert(inst->A->actors.begin(),inst->A->actors.end());
                inst->status = 1;
            }
            // executed when it is not the first time this instance is processed
            else {
                //std::cout << "moving " << b->first->name << " from B to C" << std::endl;
                auto b = inst->B.begin();
                inst->C.push_back(*b);
                //std::cout << ".";
                //inst->P.erase(b->first);
                inst->B.erase(b);
            }
            
            /*std::cout << skip << " {";
             for (auto cl: stack.at(stack.size()-1)->A->actors)
             std::cout << cl->name << " ";
             std::cout << "}" << std::endl;*/
            /* PRINT STACK: DEBUG
            for (int i=stack.size()-1; i>=0; i--) {
                std::shared_ptr<instance> inst = stack.at(i);
                std::cout << inst->skip << "  A: {";
                for (auto cl: inst->A->actors)
                std::cout << cl->name << " ";
                std::cout << "}[";
                
                for (auto la: inst->A->layers)
                std::cout << la->id << " ";
                std::cout << "] B: {";
                for (auto cl: inst->B) {
                    std::cout << cl.first->name << "[";
                    for (auto la: cl.second)
                    std::cout << la->id << " ";
                    std::cout << "] ";
                }
                std::cout << "}  C: {";
                for (auto cl: inst->C) {
                    std::cout << cl.first->name << "[";
                    for (auto la: cl.second)
                    std::cout << la->id << " ";
                    std::cout << "] ";
                }
                std::cout << "}" << std::endl;
            }
            std::cout << "++++++++++++++++++++++" << std::endl;
            */
            
            // THIS IS ALWAYS EXECUTED
            // now we can extend the current clique
            auto b = inst->B.begin();
            if (b!=inst->B.end()) {
                //std::cout << "trying extension with " << b->first->name << std::endl;
                
                // new clique
                //inst->P.insert(b->first);
                hash_set<LayerSharedPtr> S_P = s_intersection(inst->A->layers,b->second);
                hash_set<ActorSharedPtr> h;
                for (auto actor: inst->A->actors)
                    h.insert(actor);
                h.insert(b->first);
                CliqueSharedPtr A_ext(new clique(h,S_P));
                
                // new set of actors that can be used to extend the clique at the next iteration
                bool reset_skip = false;
                std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > > B_ext;
                auto q = b;
                for (++q; q!=inst->B.end(); ++q) {
                    hash_set<LayerSharedPtr> common = s_intersection(b->second,s_intersection(q->second,neighboring_layers(mnet,b->first,q->first))); //TODO encapsulate these two intersections into a single function computing them together...
                    /*std::cout << "B FILTER " << b->first->name << " " << q->first->name << "[";
                     for (auto la: common)
                     std::cout << la->id << " ";
                     std::cout << "] ";
                     std::cout << std::endl;*/
                    if (common.size()>=m) {
                        B_ext.push_back(*q);
                    }
                    if (common.size()<q->second.size()) reset_skip=true;
                }
                
                //
                std::vector<std::pair<ActorSharedPtr,hash_set<LayerSharedPtr> > > C_ext;
                for (auto c = inst->C.begin(); c!=inst->C.end(); ++c) {
                    hash_set<LayerSharedPtr> common = s_intersection(b->second,s_intersection(c->second,neighboring_layers(mnet,b->first,c->first)));
                    if (common.size()>=m) {
                        C_ext.push_back(*c);
                    }
                }
                
                /* impossible to grow the current clique to have at least k actors. stop processing it.
                if (A_ext->actors.size()+B_ext.size()<k) {
                    //std::cout << "EXT PRUNE: < k" << std::endl;
                    continue;
                }
                
                // the current clique has less than m layers. stop processing it.
                if (A_ext->layers.size()<m) {
                    //std::cout << "EXT PRUNE: < m" << std::endl;
                    continue;
                }*/
                stack.push_back(std::shared_ptr<instance>(new instance(reset_skip?1:inst->skip+1,0,A_ext,B_ext,C_ext)));
            }
            else {
                // no elements left to extend the clique - stop processing this
                for (int i=0; i< inst->skip; i++) stack.pop_back();
                //std::cout << "POP " << inst->skip << std::endl;
            }
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
                int common_actors = s_intersection(c1->actors,c2->actors).size();
                int common_layers = s_intersection(c1->layers,c2->layers).size();
                if (common_actors>=k-1 && common_layers>=m) {
                    result[c1].insert(c2);
                    result[c2].insert(c1);
                }
            }
        }
        return result;
    }

    hash_set<AdjCliqueCommunitySharedPtr> find_max_communities_max_layers(MLNetworkSharedPtr mnet, const std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> >& adjacency, size_t m) {
        // result: empty set of communities
        hash_set<AdjCliqueCommunitySharedPtr> result;
        // A: empty community on all layers
        AdjCliqueCommunitySharedPtr A = cpm_community::create();
        for (LayerSharedPtr layer: *mnet->get_layers()) {
            A->add_layer(layer);
        }
        
        hash_set<CliqueSharedPtr> AlreadySeen;
        for (auto clique_pair : adjacency) {
            //std::cout << "----NEW ITERATION----";
            AdjCliqueCommunitySharedPtr A = cpm_community::create();
            A->add_clique(clique_pair.first);
            for (LayerSharedPtr layer: clique_pair.first->layers)
                A->add_layer(layer);
            vector<CliqueSharedPtr> Candidates(clique_pair.second.begin(),clique_pair.second.end());
            layer_sets empty;
            find_max_communities_max_layers(adjacency,A,Candidates,AlreadySeen,empty,m,result);
            AlreadySeen.insert(clique_pair.first);
        }
        return result;
    }
    
    void find_max_communities_max_layers(const std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> >& adjacency, AdjCliqueCommunitySharedPtr& A,
                                         vector<CliqueSharedPtr> Candidates, hash_set<CliqueSharedPtr>& processedCliques, layer_sets& processedLayerCombinations, size_t m, hash_set<AdjCliqueCommunitySharedPtr>& result) {
        
        /*
         std::cout << "PROCESS: { ";
         for (auto b: A->cliques) {
         std::cout << b->id << "_";
         for (LayerSharedPtr l: b->layers)
         std::cout << l->id;
         std::cout << " ";
         }
         std::cout << "} ";
         for (LayerSharedPtr l: A->layers)
         std::cout << l->id;
         std::cout << std::endl;
         std::cout << "CANDIDATES: { ";
         for (auto b: Candidates) {
         std::cout << b->id << "_";
         for (LayerSharedPtr l: b->layers)
         std::cout << l->id;
         std::cout << " ";
         }
         std::cout << "} " << std::endl;
         */
        
        vector<CliqueSharedPtr> stack;
        
        // EXPAND
        while (Candidates.size()!=0) {
            CliqueSharedPtr c = Candidates.back();
            Candidates.pop_back();
            hash_set<LayerSharedPtr> i = s_intersection(A->get_layers(),c->layers);
            if (i.size()==A->get_layers().size()) {
                if (processedCliques.count(c)>0) {
                    //std::cout << "  ABORT! " << std::endl;
                    return;
                }
                A->cliques.insert(c);
                //std::cout << "  INSERT " << c->id << std::endl;
                
                for (auto j: adjacency.at(c)) {
                    if (A->cliques.count(j)>0)
                        continue;
                    else {
                        Candidates.push_back(j);
                        //std::cout << "    C <- " << j->id << std::endl;
                    }
                }
            }
            else if (i.size()>=m) {
                if (processedLayerCombinations.count(c->layers)==0) {
                    stack.push_back(c);
                    /*std::cout << "STACK! " << c->id << "_";
                     for (LayerSharedPtr l: c->layers)
                     std::cout << l->id;
                     std::cout << std::endl; */
                }
            }
            //std::cout << std::endl;
        }
        result.insert(A);
        /*std::cout << "RESULT: { ";
         for (auto b: A->cliques) {
         std::cout << b->id << "_";
         for (LayerSharedPtr l: b->layers)
         std::cout << l->id;
         std::cout << " ";
         }
         std::cout << "} ";
         for (LayerSharedPtr l: A->layers)
         std::cout << l->id;
         std::cout << std::endl;
         */
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
            find_max_communities_max_layers(adjacency,comm,stack,processedCliques,processedLayerCombinations,m,result);
        }
    }

    
}
