//
//  group_handler.h
//  group_handler
//
//  Created by Lucas Jeub on 21/11/2012.
//
// Version: 2.1
// Date: Tue 29 Nov 2016 15:29:58 EST

#ifndef __group_handler__group_handler__
#define __group_handler__group_handler__

#include "community/group_index.h"

#include <Eigen/Sparse>

#include <unordered_map>


#define NUM_TOL 1e-10

namespace mlnet {


typedef std::unordered_map<int, double> map_type;
//typedef std::map<int, double> map_type;
//typedef std::vector<double> map_type;

//map for unique possible moves
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

typedef unique_group_map set_type;

typedef std::pair<std::vector<int>,std::vector<double>> move_list;

//move node to most optimal group
double move(group_index & g, int node, Eigen::SparseMatrix<double> mod);

//move node to random group that increases modularity
//template<class M> double moverand(group_index & g, int node, const M & mod);

//move node to random group with probability proportional to increase in modularity
//template<class M> double moverandw(group_index & g, int node, const M & mod);

set_type possible_moves(group_index & g, int node, Eigen::SparseMatrix<double> mod);

map_type mod_change(group_index &g, Eigen::SparseMatrix<double> mod,set_type & unique_groups,int current_node);


move_list positive_moves(set_type & unique_groups, map_type & mod_c);


//implement unique_group_map (quick membership check and insertion of elements, quick iteration over members, unordered)
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

double move(group_index & g, int node, Eigen::SparseMatrix<double> mod){
	set_type unique_groups = possible_moves(g, node, mod);
	map_type mod_c = mod_change(g, mod, unique_groups, node);

	//find best move
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
	std::cout << mod_max << std::endl;
	if(mod_max>NUM_TOL){
		g.move(node,group_move);
		d_step+=mod_max;
	}
	return d_step;
}


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


//set up random engine
//std::default_random_engine generator((unsigned int)time(0));
/*
//move node to random group increasing modularity
template<class M> double moverand(group_index & g, int node, const M & mod){
	set_type unique_groups=possible_moves(g, node, mod);
	map_type mod_c=mod_change(g, mod, unique_groups, node);

	//find modularity increasing moves
	move_list mod_pos=positive_moves(unique_groups, mod_c);

	// move node to a random group that increases modularity
	double d_step=0;
	if (!mod_pos.first.empty()) {
		std::uniform_int_distribution<int> randindex(0,mod_pos.first.size()-1);
		int randmove=randindex(generator);
		g.move(node,mod_pos.first[randmove]);
		d_step=mod_pos.second[randmove];
	}
	return d_step;
}


//move to random group with probability proportional to increase in modularity
template<class M> double moverandw(group_index & g, int node, const M & mod){
	set_type unique_groups=possible_moves(g, node, mod);
	map_type mod_c=mod_change(g, mod, unique_groups, node);

	//find modularity increasing moves
	move_list mod_pos=positive_moves(unique_groups, mod_c);

	//move node to a random group that increases modularity with probability proportional to the increase
	double d_step=0;
	if (!mod_pos.first.empty()) {
		std::discrete_distribution<int> randindex(mod_pos.second.begin(),mod_pos.second.end());
		int randmove=randindex(generator);
		g.move(node,mod_pos.first[randmove]);
		d_step=mod_pos.second[randmove];
	}
	return d_step;
} */

}

#endif /* defined(__group_handler__group_handler__) */
