/**
 * propertymatrix
 * 
 * A sorted random map is a class used to store a set of objects that can be accessed:
 * 1. by key in (average) log time.
 * 2. by index (position) in constant time.
 * 3. by const iterating over its elements.
 * When several components of a multilayer network need to be accessed,
 * e.g., the neighbors of a node, a const reference to the corresponding
 * sorted random map is typically returned so that no objects are duplicated and
 * no additional memory is used.
 *
 * Here a sorted random map is implemented as a skip list.
 */

#ifndef MLNET_PROPERTY_MATRIX_H_
#define MLNET_PROPERTY_MATRIX_H_

#include "random.h"
#include "math.h"
#include "../exceptions.h"
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <array>
#include <memory>
#include <cmath>
#include <algorithm>

namespace mlnet {

/**

 */
template <class STRUCTURE, class CONTEXT, class VALUE>
class property_matrix {

private:
	simple_set<STRUCTURE> structures;
	simple_set<CONTEXT> contexts;
	hashtable<CONTEXT,hashtable<STRUCTURE,VALUE> > data;
	VALUE default_value;

public:
	const long num_rows;
	const long num_cols;

	/**
	 * Creates a property matrix with a given number of columns and rows.
	 */
	property_matrix(long num_rows, long num_cols, VALUE default_value);

    VALUE get(const STRUCTURE&, const CONTEXT&) const;

    void set(const STRUCTURE&, const CONTEXT&, VALUE);

    void rankify(const CONTEXT&);

    const simple_set<CONTEXT>& rows() const;

    const simple_set<STRUCTURE>& columns() const;
};

/* TEMPLATE CODE */

template <class STRUCTURE, class CONTEXT, class VALUE>
property_matrix<STRUCTURE,CONTEXT,VALUE>::property_matrix(long num_rows, long num_cols, VALUE default_value) :
default_value(default_value), num_rows(num_rows), num_cols(num_cols) {}

template <class STRUCTURE, class CONTEXT, class VALUE>
VALUE property_matrix<STRUCTURE,CONTEXT,VALUE>::get(const STRUCTURE& s, const CONTEXT& c) const {
	if (data.count(c)==0)
		return default_value;
	if (data.at(c).count(s)==0)
		return default_value;
	return data.at(c).at(s);
}

template <class STRUCTURE, class CONTEXT, class VALUE>
void property_matrix<STRUCTURE,CONTEXT,VALUE>::set(const STRUCTURE& s, const CONTEXT& c, VALUE v) {
	data[c][s] = v;
	contexts.insert(c); // TODO this might slow down the function significantly - check
	structures.insert(s);
}

template <class STRUCTURE, class CONTEXT, class VALUE>
const simple_set<CONTEXT>& property_matrix<STRUCTURE,CONTEXT,VALUE>::rows() const {
	return contexts;
}

template <class STRUCTURE, class CONTEXT, class VALUE>
const simple_set<STRUCTURE>& property_matrix<STRUCTURE,CONTEXT,VALUE>::columns() const {
	return structures;
}

struct binary_vector_comparison {
public:
	long yy = 0;
	long yn = 0;
	long ny = 0;
	long nn = 0;
};


template <class STRUCTURE, class CONTEXT>
binary_vector_comparison compare(property_matrix<STRUCTURE,CONTEXT,bool> P, const CONTEXT& c1, const CONTEXT& c2) {
	binary_vector_comparison res;
	res.yy = 0;
	res.yn = 0;
	res.ny = 0;
	res.nn = 0;

	long checked_columns = 0;

	for (STRUCTURE s: P.columns()) {
		bool p1 = P.get(s,c1);
		bool p2 = P.get(s,c2);
		if (p1 && p2) res.yy++;
		else if (p1) res.yn++;
		else if (p2) res.ny++;
		else res.nn++;
		checked_columns++;
	}
	res.nn += P.num_cols - checked_columns;
	return res;
}

template <class STRUCTURE, class CONTEXT>
double russell_rao(property_matrix<STRUCTURE,CONTEXT,bool> P, const CONTEXT& c1, const CONTEXT& c2) {
	binary_vector_comparison comp = compare(P, c1, c2);
	return (double)(comp.yy)/(comp.yy+comp.ny+comp.yn+comp.nn);
}

template <class STRUCTURE, class CONTEXT>
double jaccard(property_matrix<STRUCTURE,CONTEXT,bool> P, const CONTEXT& c1, const CONTEXT& c2) {
	binary_vector_comparison comp = compare(P, c1, c2);
	return (double)comp.yy/(comp.yy+comp.yn+comp.ny);
}

template <class STRUCTURE, class CONTEXT>
double coverage(property_matrix<STRUCTURE,CONTEXT,bool> P, const CONTEXT& c1, const CONTEXT& c2) {
	binary_vector_comparison comp = compare(P, c1, c2);
	return (double)comp.yy/(comp.yy+comp.yn);
}

template <class STRUCTURE, class CONTEXT>
double kulczynski2(property_matrix<STRUCTURE,CONTEXT,bool> P, const CONTEXT& c1, const CONTEXT& c2) {
	binary_vector_comparison comp = compare(P, c1, c2);
	return ((double)comp.yy/(comp.yy+comp.yn)+comp.yy/(comp.yy+comp.ny))/2;
}

template <class STRUCTURE, class CONTEXT>
double simple_matching(property_matrix<STRUCTURE,CONTEXT,bool> P, const CONTEXT& c1, const CONTEXT& c2) {
	binary_vector_comparison comp = compare(P, c1, c2);
	return (double)(comp.yy+comp.nn)/(comp.yy+comp.ny+comp.yn+comp.nn);
}

template <class STRUCTURE, class CONTEXT>
double pearson(property_matrix<STRUCTURE,CONTEXT,long> P, const CONTEXT& c1, const CONTEXT& c2) {
	double cov = 0;
	double mean1 = 0;
	double mean2 = 0;
	double std1 = 0;
	double std2 = 0;
	long checked_columns = 0;

	for (STRUCTURE s:  P.columns()) {
		mean1 += P.get(s,c1);
		mean2 += P.get(s,c2);
		checked_columns++;
	}
	mean1 /= checked_columns;
	mean2 /= checked_columns;
	for (STRUCTURE s: P.columns()) {
		long val1 = P.get(s,c1);
		long val2 = P.get(s,c2);
		cov += (val1-mean1)*(val2-mean2);
		std1 += (val1-mean1)*(val1-mean1);
		std2 += (val2-mean2)*(val2-mean2);
	}
	if (std1==0 && std2==0)
		return 1;
	else if (std1==0 || std2==0)
		return 0;
	else return cov/std1/std2;
}


template <class STRUCTURE, class CONTEXT, class NUMBER>
class structure_comparison_function {
public:
	structure_comparison_function(const property_matrix<STRUCTURE,CONTEXT,NUMBER>* P, const CONTEXT* c)
	: P(P), c(c) {}
	const property_matrix<STRUCTURE,CONTEXT,NUMBER>* P;
	const CONTEXT* c;
	bool operator()(const STRUCTURE& s1, const STRUCTURE& s2) const {
		return (*P).get(s1,*c)<(*P).get(s2,*c);
	}
};

template <class STRUCTURE, class CONTEXT, class NUMBER>
void property_matrix<STRUCTURE,CONTEXT,NUMBER>::rankify(const CONTEXT& c) {
	std::vector<STRUCTURE> ranks(structures.begin(),structures.end());

	structure_comparison_function<STRUCTURE,CONTEXT,NUMBER> f(this,&c);
	std::sort(ranks.begin(), ranks.end(), f);

	for (int i=0; i<ranks.size(); i++) {
		set(ranks[i],c,i);
	}
}

} // namespace mlnet

#endif /* MLNET_PROPERTY_MATRIX_H_ */
