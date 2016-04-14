/**
 * math.h
 *
 * Basic mathematical functions and conversion from/to numbers/strings.
 *
 */

#ifndef MLNET_MATH_H_
#define MLNET_MATH_H_

#include "exceptions.h"
#include <unordered_set>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>

namespace mlnet {

double mean(const std::vector<double>& vec);
double stdev(const std::vector<double>& vec);


template <class T>
std::unordered_set<T> intersect(std::unordered_set<T> set1, std::unordered_set<T> set2) {
	std::unordered_set<T> result;
	for (T l: set1) {
		if (set2.count(l)==1) {
			result.insert(l);
		}
	}
	return result;
}

template <class T>
std::unordered_set<T> set_union(std::unordered_set<T> set1, std::unordered_set<T> set2) {
	std::unordered_set<T> result;
	result.insert(set1.begin(), set1.end());
	result.insert(set2.begin(), set2.end());
	return result;
}

template <typename T> std::string to_string (T Number) {
	std::ostringstream ss;
    ss << Number;
    return ss.str();
}

double to_double(const std::string &double_as_string);

}

#endif /* MLNET_MATH_H_ */
