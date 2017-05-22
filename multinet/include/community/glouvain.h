#ifndef GLOUVAIN_H_
#define GLOUVAIN_H_

#include "cutils.h"
#include <list>

namespace mlnet {

class glouvain {

public:

	/*
		Use : glouvain g;
			 g.get_ml_community(MLNetworkSharedPtr);

		Data invariant:
		Generalized Louvain is a multiplex community detector based on the Louvain community detection
		method for single layer networks. This implementation is based from http://netwiki.amath.unc.edu/GenLouvain/GenLouvain
	*/
	CommunityStructureSharedPtr fit(MLNetworkSharedPtr mnet, std::string m, double gamma, double omega);
	Eigen::SparseMatrix<double> metanetwork(Eigen::SparseMatrix<double> B, std::vector<int> S2);

	/* Map indexes of b to values of a:
	https://stackoverflow.com/questions/5691218/matlab-mapping-values-to-index-of-other-array*/
	std::vector<int> mapV2I(std::vector<int> a, std::vector<int> b);
	double Q(Eigen::SparseMatrix<double> M, std::vector<int> y, double twoum);

};


struct unique_group_map {
		unique_group_map();
		unique_group_map(size_t n);
		std::vector<bool> ismember;
		std::vector<int> members;
		bool count(int i);
		void insert(int i);
		typedef std::vector<int>::iterator iterator;
		iterator begin();
		iterator end();
	};

#define NUM_TOL 1e-100

typedef std::unordered_map<int, double> map_type;
//map for unique possible moves
typedef unique_group_map set_type;

typedef std::pair<std::vector<int>,std::vector<double>> move_list;

struct group_index {
	group_index():n_nodes(0), n_groups(0){};
	group_index(std::vector<int> v) {
		n_nodes = v.size();
		nodes.resize(n_nodes);
		nodes_iterator.resize(n_nodes);

		for(size_t i = 0; i < n_nodes; i++){
			nodes[i] = v[i];
		}

		n_groups = *max_element(nodes.begin(),nodes.end()) + 1;
		groups.resize(n_groups);

		for(size_t i = 0; i <n_nodes; i++){
			groups[nodes[i]].push_back(i);
			nodes_iterator[i]= --groups[nodes[i]].end();
		}
	};

	//move node to group
	void move(int node, int group){
		//move node by splicing into list for new group
		groups[group].splice(groups[group].end(), groups[nodes[node]],nodes_iterator[node]);
		//update its group asignment
		nodes[node]=group;
	};

	size_t n_nodes;
	size_t n_groups;

	std::vector<int> toVector() {
		vector<int> v (n_nodes);
		vector<bool> track_move(n_nodes, true);
		size_t g_n = 0;

		std::list<int>::iterator it;

		for(size_t i = 0; i < n_nodes; i++){
			if(track_move[i]){
				for(it=groups[nodes[i]].begin(); it != groups[nodes[i]].end();it++){
					v[*it] = g_n;
					track_move[*it] = false;
				}
				g_n++;
			}
		}
		return v;
	};

	std::vector<std::list<int>> groups; //the index of each node in a group is stored in a linked list
	std::vector<std::list<int>::iterator> nodes_iterator; //stores the position of the node in the list for the group it belongs to
	std::vector<int> nodes; //stores the group a node belongs to
};

unique_group_map::unique_group_map() : ismember(std::vector<bool>()) {}
unique_group_map::unique_group_map(size_t n) : ismember(std::vector<bool>(n,false)) {}
bool unique_group_map::count(int i) {return ismember[i];}
void unique_group_map::insert(int i) {
	if (!ismember[i]) {
		ismember[i] = true;
		members.push_back(i);
	}
}

unique_group_map::iterator unique_group_map::begin() { return members.begin(); }
unique_group_map::iterator unique_group_map::end() { return members.end(); }

// Random engine used for random movement functions
std::default_random_engine generator((unsigned int)time(0));

set_type possible_moves(group_index & g, int node, Eigen::SparseMatrix<double> mod){
		set_type unique_groups(g.n_groups);
		unique_groups.insert(g.nodes[node]);
		//add nodes with potential positive contribution to unique_groups
		for (int i = 0; i < mod.outerSize(); i++) {
			for (Eigen::SparseMatrix<double>::InnerIterator it(mod, i); it; ++it) {
				unique_groups.insert(g.nodes[it.row()]);
			}
		}

		return unique_groups;
	}

//calculates changes in modularity for sparse modularity matrix
map_type mod_change(group_index & g, Eigen::SparseMatrix<double> mod, set_type & unique_groups, int current_node){
	int current_group = g.nodes[current_node];
	map_type mod_c;

	double mod_current = mod.coeff(current_node, 0);

	for(set_type::iterator it=unique_groups.begin(); it!=unique_groups.end();++it){
		mod_c[*it]=0;
	}

	for (int i = 0; i < mod.outerSize(); i++) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(mod, i); it; ++it) {
			if (unique_groups.count(g.nodes[it.row()])) {
				mod_c[g.nodes[it.row()]] += it.value();
			}
		}
	}

	mod_c[current_group]-=mod_current;
	mod_current=mod_c[current_group];
	for (set_type::iterator it=unique_groups.begin(); it!=unique_groups.end(); ++it) {
		mod_c[*it]-= mod_current;
	}

	return mod_c;
}

//find moves that improve modularity
move_list positive_moves(set_type & unique_groups, map_type & mod_c){
	move_list moves;
	for(set_type::iterator it=unique_groups.begin();it!=unique_groups.end();++it){
		if(mod_c[*it]>NUM_TOL){
			moves.first.push_back(*it);
			moves.second.push_back(mod_c[*it]);
		}
	}
	return moves;
}

//move best move
double move(group_index & g, int node, Eigen::SparseMatrix<double> mod){
	set_type unique_groups = possible_moves(g, node, mod);
	map_type mod_c = mod_change(g, mod, unique_groups, node);

	double mod_max=0;
	double d_step=0;
	int group_move = g.nodes[node]; //stay in current group if no improvement
	for(set_type::iterator it=unique_groups.begin();it!=unique_groups.end();++it){
		if(mod_c[*it]>mod_max){
			mod_max=mod_c[*it];
			group_move=*it;
		}
	}
	//move current node to most optimal group
	if(mod_max>NUM_TOL){
		g.move(node,group_move);
		d_step+=mod_max;
	}
	return d_step;
}

//move to random group with probability proportional to increase in modularity
double moverandw(group_index & g, int node, Eigen::SparseMatrix<double> mod) {
	set_type unique_groups = possible_moves(g, node, mod);
	map_type mod_c = mod_change(g, mod, unique_groups, node);

	//find modularity increasing moves
	move_list mod_pos = positive_moves(unique_groups, mod_c);

	//move node to a random group that increases modularity with probability proportional to the increase
	double d_step=0;
	if (!mod_pos.first.empty()) {
		std::discrete_distribution<int> randindex(mod_pos.second.begin(),mod_pos.second.end());
		int randmove = randindex(generator);
		g.move(node, mod_pos.first[randmove]);
		d_step = mod_pos.second[randmove];
	}
	return d_step;
}



}

#endif
