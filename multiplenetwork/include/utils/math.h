/**
 * math.h
 *
 * Basic mathematical functions and conversion from/to numbers/strings.
 *
 */

#ifndef MLNET_MATH_H_
#define MLNET_MATH_H_

#include "../exceptions.h"
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>

namespace mlnet {

template <class T> using simple_set = std::unordered_set<T>;
template <class T> using sorted_set = std::set<T>;
//template <class K, class V> using simple_map = std::unordered_map<K,V>;
template <class K, class V> using sorted_map = std::map<K,V>;
template <class T> using vector = std::vector<T>;
template <class T> using matrix = std::vector<std::vector<T> >;
template <class T1, class T2> using hashtable = std::unordered_map<T1,T2>;

double mean(const std::vector<double>& vec);
double stdev(const std::vector<double>& vec);

template <class T>
double jaccard_similarity(const std::vector<simple_set<T> >& sets) {
	long union_size = s_union(sets).size();
	if (union_size==0)
		return 0;
	long intersection_size = s_intersection(sets).size();
	return (double)intersection_size/union_size;
}

template <class T>
simple_set<T> s_intersection(const std::vector<simple_set<T> >& sets) {
	simple_set<T> result;
	int idx = 0; // index of the smallest set
	for (int i=1; i<sets.size(); i++) {
		if (sets.at(i).size() < sets.at(idx).size()) {
			idx=1;
		}
	}
	for (T element: sets.at(idx)) {
		bool in_intersection = true;
		for (int i=0; i<sets.size(); i++) {
			if (i==idx) continue;
			if (sets.at(i).count(element)==0) {
				in_intersection = false;
				break;
			}
		}
		if (in_intersection) {
			result.insert(element);
		}
	}
	return result;
}

// TODO can be made more efficient exploiting sorting
template <class T>
simple_set<T> s_intersection(const std::vector<sorted_set<T> >& sets) {
	simple_set<T> result;
	int idx = 0; // index of the smallest set
	for (int i=1; i<sets.size(); i++) {
		if (sets.at(i).size() < sets.at(idx).size()) {
			idx=1;
		}
	}
	for (T element: sets.at(idx)) {
		bool in_intersection = true;
		for (int i=0; i<sets.size(); i++) {
			if (i==idx) continue;
			if (sets.at(i).count(element)==0) {
				in_intersection = false;
				break;
			}
		}
		if (in_intersection) {
			result.insert(element);
		}
	}
	return result;
}

template <class T>
simple_set<T> s_intersection(const simple_set<T>& set1, const simple_set<T>& set2) {
	std::vector<simple_set<T> > sets({set1,set2});
	return s_intersection(sets);
}

template <class T>
simple_set<T> s_intersection(const sorted_set<T>& set1, const sorted_set<T>& set2) {
	std::vector<sorted_set<T> > sets({set1,set2});
	return s_intersection(sets);
}

template <class T>
simple_set<T> s_intersection(const sorted_set<T>& set1, const simple_set<T>& set2) {
	simple_set<T> result;
	for (T element: set1) {
		if (set2.count(element)>0) {
			result.insert(element);
		}
	}
	return result;
}

template <class T>
simple_set<T> s_intersection(const simple_set<T>& set1, const sorted_set<T>& set2) {
	simple_set<T> result;
	for (T element: set2) {
		if (set1.count(element)>0) {
			result.insert(element);
		}
	}
	return result;
}

template <class T>
simple_set<T> s_union(const std::vector<simple_set<T> >& sets) {
	simple_set<T> result;
	for (simple_set<T> S: sets) {
		result.insert(S.begin(), S.end());
	}
	return result;
}

template <class T>
simple_set<T> s_union(const simple_set<T>& set1, const simple_set<T>& set2) {
	simple_set<T> result;
	result.insert(set1.begin(), set1.end());
	result.insert(set2.begin(), set2.end());
	return result;
}

template <typename T> std::string to_string (T number) {
	std::ostringstream ss;
    ss << number;
    return ss.str();
}

template <typename T> std::string to_string (const simple_set<T>& set) {
	std::ostringstream ss;
    ss << "{ ";
    for (T element: set)
    	ss << element << " ";
    ss << "}";
    return ss.str();
}

template <typename T> std::string to_string (const sorted_set<T>& set) {
	std::ostringstream ss;
    ss << "{ ";
    for (T element: set)
    	ss << element << " ";
    ss << "}";
    return ss.str();
}

double to_double(const std::string &double_as_string);

}

#endif /* MLNET_MATH_H_ */
