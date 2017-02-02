/*
 * ML-Clique percolation method
 *
 */

#include "utils.h"
#include <iostream>
#include <sys/time.h>

#include "community.h"

namespace mlnet {

int t_a;
int t_b;
struct timeval t1, t2;

cpm_community::cpm_community() :
	id(0) {}

cpm_community::cpm_community(long id, hash_set<CliqueSharedPtr> cliques, hash_set<LayerSharedPtr> layers) :
	id(id), cliques(cliques.begin(),cliques.end()), layers(layers.begin(),layers.end()) {}

std::set<ActorSharedPtr> cpm_community::actors() {
	std::set<ActorSharedPtr> actors;
	for (CliqueSharedPtr clique: cliques) {
		for (ActorSharedPtr actor: clique->actors)
			actors.insert(actor);
	}
	return actors;
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

hash_set<CommunitySharedPtr> ml_cpm(MLNetworkSharedPtr mnet, int k, int m1, int m2, int m3) {
	/*hash_set<CliqueSharedPtr> C = find_max_cliques(mnet,k,m1);
	int i=0;
	for (CliqueSharedPtr c: C) {
		std::cout << (i++) << " " << c->to_string() << std::endl;
	}
	i=0;*/
	hash_set<CliqueSharedPtr> C = find_max_cliques_it(mnet,k,m1);
	/*for (CliqueSharedPtr c: C) {
		std::cout << (i++) << " " << c->to_string() << std::endl;
	}*/

	if (C.size()==0) return hash_set<CommunitySharedPtr>();
	std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> > adjacency = build_max_adjacency_graph(C,k,m2);
	return find_max_communities_max_layers(mnet,adjacency,m3);
}

hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > get_mlneighbors(MLNetworkSharedPtr mnet, ActorSharedPtr actor) {
	hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > result;
	for (NodeSharedPtr node: *mnet->get_nodes(actor)) {
		for (NodeSharedPtr neigh: *mnet->neighbors(node,INOUT)) {
			result[neigh->actor].insert(neigh->layer);
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



hash_set<CliqueSharedPtr> find_max_cliques(MLNetworkSharedPtr mnet, int k, int m) {
	hash_set<LayerSharedPtr> layers;
	for (LayerSharedPtr layer: *mnet->get_layers())
		layers.insert(layer);
	return find_max_cliques(mnet, layers, k, m);
}

/*hash_set<CliqueSharedPtr> find_max_cliques_it(MLNetworkSharedPtr mnet, int k, int m) {
	hash_set<LayerSharedPtr> layers;
	for (LayerSharedPtr layer: *mnet->get_layers())
		layers.insert(layer);
	return find_max_cliques_it(mnet, layers, k, m);
}*/


hash_set<CliqueSharedPtr> find_max_cliques(MLNetworkSharedPtr mnet, hash_set<LayerSharedPtr> layers, int k, int m) {
	hash_set<CliqueSharedPtr> result;
	// A: empty clique
	CliqueSharedPtr A(new clique());
	A->layers.insert(layers.begin(), layers.end());
	// B: all actors
	hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > B;
	for (ActorSharedPtr actor: *mnet->get_actors()) {
		//std::cout << "Processing actor: " << actor->name << std::endl;
		B[actor] = layers;
	}
	// C: empty set of actors
	hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > C;
	hash_set<ActorSharedPtr> empty;
	find_max_cliques(mnet,A,B,C,empty,result,k,m);
	return result;
}


/*hash_set<CliqueSharedPtr> find_max_cliques_it(MLNetworkSharedPtr mnet, int k, int m) {
	hash_set<CliqueSharedPtr> result;
	// A: empty clique
	CliqueSharedPtr A(new clique());
	for (LayerSharedPtr layer: *mnet->get_layers())
		A->layers.insert(layer);
	// B: all actors
	hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > B;
	for (ActorSharedPtr actor: *mnet->get_actors()) {
		for (NodeSharedPtr node: *mnet->get_nodes(actor)) {
			B[actor].insert(node->layer);
		}
	}
	// C: empty set of actors
	hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > C;
	find_max_cliques_it(mnet,A,B,C,result,k,m);
	return result;
}*/


void find_max_cliques(MLNetworkSharedPtr mnet, CliqueSharedPtr& A,
hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> >& B,
hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> >& C,
hash_set<ActorSharedPtr>& in_clique,
hash_set<CliqueSharedPtr>& result, int k, int m) {

	/*
	std::cout << "CALL "<< std::endl;
	std::cout << "  A: ";
	for (auto cl: A->actors)
		std::cout << cl->name << " ";
	std::cout << std::endl;
	std::cout << "  B: ";
	for (auto cl: B)
		std::cout << cl.first->name << " ";
	std::cout << std::endl;
	std::cout << "  C: ";
	for (auto cl: C)
		std::cout << cl.first->name << " ";
	std::cout << std::endl;
*/

	if (A->actors.size()>=k) {
		int max_layers_C = 0;
		for (auto c: C) {
			if (c.second.size()>max_layers_C)
				max_layers_C = c.second.size();
		}
		int max_layers_B = 0;
		for (auto b: B) {
			if (b.second.size()>max_layers_B)
				max_layers_B = b.second.size();
		}
		if (max_layers_B<A->layers.size() && max_layers_C<A->layers.size()) {
			//A->id = result.size();
			result.insert(A);
			in_clique.insert(A->actors.begin(),A->actors.end());


			//std::cout << "RETURN" << std::endl;
			// TMP
			//A->id = result.size();
		}
	}

	hash_set<ActorSharedPtr> P(A->actors.begin(),A->actors.end());

	for (auto b = B.begin(); b!=B.end(); ++b) {
		// for all b in B
		if (in_clique.count(b->first)>0) {
			//std::cout << " nn " << b->first->name << std::endl;
			continue;
		}
		P.insert(b->first);
		hash_set<LayerSharedPtr> S_P = s_intersection(A->layers,b->second);
		CliqueSharedPtr A_ext(new clique(P,S_P));


		hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > B_ext;
		auto q = b;
		for (++q; q!=B.end(); ++q) {
			hash_set<LayerSharedPtr> common = s_intersection(q->second,neighboring_layers(mnet,b->first,q->first));
			if (common.size()>=m) {
				B_ext[q->first] = common;
			}
		}

		hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > C_ext;
		for (auto c = C.begin(); c!=C.end(); ++c) {
			hash_set<LayerSharedPtr> common = s_intersection(c->second,neighboring_layers(mnet,b->first,c->first));
			if (common.size()>=m) {
				C_ext[c->first] = common;
			}
		}

		hash_set<ActorSharedPtr> empty;
		if ((A_ext->actors.size()+B_ext.size()>=k) & (A_ext->layers.size()>=m))
			find_max_cliques(mnet,A_ext,B_ext,C_ext,empty,result,k,m);
		in_clique.insert(empty.begin(),empty.end());

		C[b->first] = b->second;
		//std::cout << ".";
		P.erase(b->first);
	}
}

std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> > build_max_adjacency_graph(const hash_set<CliqueSharedPtr>& C, int k, int m) {
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

hash_set<CommunitySharedPtr> find_max_communities_max_layers(MLNetworkSharedPtr mnet, const std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> >& adjacency, int m) {
	// result: empty set of communities
	hash_set<CommunitySharedPtr> result;
	// A: empty community on all layers
	CommunitySharedPtr A(new cpm_community());
	hash_set<LayerSharedPtr> all_layers;
	for (LayerSharedPtr layer: *mnet->get_layers()) {
		all_layers.insert(layer);
	}
	A->layers.insert(all_layers.begin(),all_layers.end());

	hash_set<CliqueSharedPtr> AlreadySeen;
	for (auto clique_pair : adjacency) {
		//std::cout << "----NEW ITERATION----";
		CommunitySharedPtr A(new cpm_community());
		A->cliques.insert(clique_pair.first);
		A->layers.insert(clique_pair.first->layers.begin(),clique_pair.first->layers.end());
		vector<CliqueSharedPtr> Candidates(clique_pair.second.begin(),clique_pair.second.end());
		layer_sets empty;
		find_max_communities_max_layers(adjacency,A,Candidates,AlreadySeen,empty,m,result);
		AlreadySeen.insert(clique_pair.first);
	}
	return result;
}

void find_max_communities_max_layers(const clique_adjacency_graph& adjacency, CommunitySharedPtr& A,
		vector<CliqueSharedPtr> Candidates, hash_set<CliqueSharedPtr>& processedCliques, layer_sets& processedLayerCombinations, int m, hash_set<CommunitySharedPtr>& result) {

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
		hash_set<LayerSharedPtr> i = s_intersection(A->layers,c->layers);
		if (i.size()==A->layers.size()) {
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
	processedLayerCombinations.insert(A->layers);

	layer_sets candidate_layer_combinations;
	for (CliqueSharedPtr c: stack) {
		hash_set<LayerSharedPtr> s = s_intersection(A->layers,c->layers);
		sorted_set<LayerSharedPtr> to_be_processed(s.begin(), s.end()); // FIXME
		candidate_layer_combinations.insert(to_be_processed);
	}
	for (sorted_set<LayerSharedPtr> layers: candidate_layer_combinations) {
		CommunitySharedPtr comm(new cpm_community());
		comm->cliques.insert(A->cliques.begin(),A->cliques.end());
		comm->layers.insert(layers.begin(),layers.end());
		find_max_communities_max_layers(adjacency,comm,stack,processedCliques,processedLayerCombinations,m,result);
	}
}


/*

hash_map<NodeSharedPtr,long> label_propagation(const MLNetworkSharedPtr& mnet, double c) {

hash_map<NodeSharedPtr,long> membership;
	// initial assignment
	long i=0;
	for (NodeSharedPtr node: mnet->get_nodes()) {
		membership[node] = i++;
	}

hash_map<LayerSharedPtr,long> m_s;
	for (LayerSharedPtr s: mnet->get_layers()) {
		double m = mnet->get_edges(s,s).size();
		if (!mnet->is_directed(s,s))
			m *= 2;
		if (m == 0)
			m = 1; // no effect on the formula
		m_s[s] = m;
	}
	while (!stopping_condition(mnet,update_labels(mnet,membership,m_s,c,100))) {}
	return membership;
}

long update_labels(const MLNetworkSharedPtr& mnet,
hash_map<NodeSharedPtr,long>& membership,
hash_map<LayerSharedPtr,long> m_s, double c, long max_neighbors) {
	long num_updates = 0;
	for  (NodeSharedPtr node: mnet->get_nodes()) {
		if (mnet->neighbors(node,INOUT).size()==0)
			continue;

hash_map<long, double> labels;
		long k_node = mnet->neighbors(node,INOUT).size();
		for (NodeSharedPtr n: mnet->neighbors(node,INOUT)) {
			/ *
			int checked_neighbors = 0;
			for (NodeSharedPtr n2: mnet->neighbors(node,INOUT)) {
				if (max_neighbors<=checked_neighbors++)
					break;
				if (mnet->neighbors(n1,INOUT).get(n2->id))
					labels.inc(membership.at(n1));
			}
			* /
			long label = membership.at(n);
			long k_n = mnet->neighbors(n,INOUT).size();
			if (labels.count(label)==0)
				labels[label]=0;
			labels[label] += 1 - k_n*k_node/m_s.at(node->layer);
		}

		for (NodeSharedPtr n: mnet->get_nodes(node->actor)) {
			if (n==node) continue;
			long label = membership.at(n);
			if (labels.count(label)==0)
				labels[label]=0;
			labels[label] += c;
		}
		// find max label
		long new_label = 0;
		double max_val = 0;
		for (auto pair: labels) {
			if (max_val < pair.second) {
				max_val = pair.second;
				new_label = pair.first;
			}
		}
		if (membership[node] != new_label) {
			membership[node] = new_label;
			num_updates++;
		}
	}
	return num_updates;
}

bool stopping_condition(const MLNetworkSharedPtr& mnet, long info) {
	if (info<mnet->get_nodes().size()*.05) return true;
	else return false;
}
*/


/*** iterative versions ***/

/**
 * This method finds all the maximal cliques (1) being a superset of clique A
 * (2) extended with actors in B (3) not containing actors in C (4) on
 * at least k actors and m layers.
 */
hash_set<CliqueSharedPtr> find_max_cliques_it(MLNetworkSharedPtr mnet, int k, int m) {

	hash_set<CliqueSharedPtr> result;
	// A: empty clique
	CliqueSharedPtr A(new clique());
	for (LayerSharedPtr layer: *mnet->get_layers())
		A->layers.insert(layer);
	// B: all actors
	hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > B;
	for (ActorSharedPtr actor: *mnet->get_actors()) {
		for (NodeSharedPtr node: *mnet->get_nodes(actor)) {
			B[actor].insert(node->layer);
		}
	}
	// C: empty set of actors
	hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > C;

	struct instance {
		// This is used to determine whether this is the first time we process this instance (status=0)
		int status;
		CliqueSharedPtr A;
		hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > B;
		hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > C;
		hash_set<ActorSharedPtr> P;
		instance(int status, CliqueSharedPtr& A, hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> >& B,
				hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> >& C) : status(status), A(A), B(B), C(C) {}
	};

	vector<std::shared_ptr<instance> > stack;
	stack.push_back(std::shared_ptr<instance>(new instance(0,A,B,C)));

	while (!stack.empty()) {

		//std::cout << "---------------------" << std::endl;
		std::shared_ptr<instance> inst = stack.back();

		// THIS IS EXECUTED ONLY THE FIRST TIME THIS IS PROCESSED
		if (inst->status==0) {
			// impossible to grow the current clique to have at least k actors. stop processing it.
			if (inst->A->actors.size()+inst->B.size()<k) {
				//std::cout << "STOP: less than K actors available" << std::endl;
				stack.pop_back();
				continue;
			}

			// the current clique has less than m layers. stop processing it.
			if (inst->A->layers.size()<m) {
				//std::cout << "STOP: less than M layers" << std::endl;
				stack.pop_back();
				continue;
			}

			// Compute the maximum number of layers for elements in B and C
			int max_layers_C = 0;
			for (auto c: inst->C) {
				if (c.second.size()>max_layers_C)
					max_layers_C = c.second.size();
			}
			int max_layers_B = 0;
			for (auto b: inst->B) {
				if (b.second.size()>max_layers_B)
					max_layers_B = b.second.size();
			}
			// return the current clique if:
			// (1) it has at least k actors
			// (2) it cannot be extended by any new actor maintaining the current number of layers
			// (3) it cannot be extended by an already processed actors maintaining the current number of layers (that is: it has been returned before)
			if (inst->A->actors.size()>=k &&
					max_layers_B < inst->A->layers.size() &&
					max_layers_C < inst->A->layers.size()) {
					//A->id = result.size();
					result.insert(inst->A);
					//for (int i=0; i<A->actors.size(); i++) std::cout << " - ";
					//std::cout << "RETURN " << inst->A->to_string() << std::endl;
					// TMP
					//inst->A->id = result.size();
			}
			// ONLY FOR DEBUG:
			//if (!(inst->A->actors.size()>=k)) std::cout << "NOT ENOUGH ACTORS / " << std::endl;
			//if (!(max_layers_B < inst->A->layers.size())) std::cout << "CAN STILL BE EXTENDED ON THE SAME LAYERS / " << std::endl;
			//if (!(max_layers_C < inst->A->layers.size())) std::cout << "PART OF AN ALREADY RETURNED CLIQUE" << std::endl;


			// we keep processing this, because it can still grow on at least m layers
			inst->P.insert(inst->A->actors.begin(),inst->A->actors.end());
			inst->status = 1;
		}
		// THIS IS EXECUTED INSTEAD
		else {
			hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> >::iterator b = inst->B.begin();
			//std::cout << "moving " << b->first->name << " from B to C" << std::endl;
			inst->C[b->first] = b->second;
			//std::cout << ".";
			inst->P.erase(b->first);
			inst->B.erase(b->first);
		}

		/* PRINT STACK: DEBUG
		for (int i=stack.size()-1; i>=0; i--) {
			std::shared_ptr<instance> inst = stack.at(i);
			std::cout << inst->status << "  A: {";
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
			inst->P.insert(b->first);
			hash_set<LayerSharedPtr> S_P = s_intersection(inst->A->layers,b->second);
			CliqueSharedPtr A_ext(new clique(inst->P,S_P));

			// new set of actors that can be used to extend the clique at the next iteration
			hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > B_ext;
			auto q = b;
			for (++q; q!=inst->B.end(); ++q) {
				hash_set<LayerSharedPtr> common = s_intersection(b->second,s_intersection(q->second,neighboring_layers(mnet,b->first,q->first)));
				/*std::cout << "B FILTER " << b->first->name << " " << q->first->name << "[";
				for (auto la: common)
						std::cout << la->id << " ";
				std::cout << "] ";
				std::cout << std::endl;*/
				if (common.size()>=m) {
					B_ext[q->first] = common;
				}
			}

			//
			hash_map<ActorSharedPtr,hash_set<LayerSharedPtr> > C_ext;
			for (auto c = inst->C.begin(); c!=inst->C.end(); ++c) {
				hash_set<LayerSharedPtr> common = s_intersection(b->second,s_intersection(c->second,neighboring_layers(mnet,b->first,c->first)));
				if (common.size()>=m) {
					C_ext[c->first] = common;
				}
			}

			// impossible to grow the current clique to have at least k actors. stop processing it.
			if (A_ext->actors.size()+B_ext.size()<k) {
				//std::cout << "EXT PRUNE: < k" << std::endl;
				continue;
			}

			// the current clique has less than m layers. stop processing it.
			if (A_ext->layers.size()<m) {
				//std::cout << "EXT PRUNE: < m" << std::endl;
				continue;
			}
			stack.push_back(std::shared_ptr<instance>(new instance(0,A_ext,B_ext,C_ext)));
		}
		else {
			// no elements left to extend the clique - stop processing this
			stack.pop_back();
		}
	}
	return result;
}

}
