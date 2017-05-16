/**
 * math.h
 *
 * This header defines:
 * - Aliases for basic mathematical entities (sets, maps, vectors, matrices).
 * - Basic mathematical/statistical functions (mean, standard deviation).
 * - Conversions from (to) numbers to (from) strings.
 */

#ifndef MLNET_MATH_H_
#define MLNET_MATH_H_

#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace mlnet {

/** A mathematical set, with no guaranteed ordering of its elements */
template <class T> using hash_set = std::unordered_set<T>;
/** A mathematical set whose elements are kept ordered */
template <class T> using sorted_set = std::set<T>;
/** A map with no guaranteed ordering of its elements, implemented as a hash table */
template <class T1, class T2> using hash_map = std::unordered_map<T1,T2>;
/** A map whose keys are kept ordered */
template <class K, class V> using sorted_map = std::map<K,V>;
/** A vector */
template <class T> using vector = std::vector<T>;
/** A matrix */
template <class T> using matrix = std::vector<std::vector<T> >;

/**
 * Mathematical mean (sum of elements divided by number of elements).
 * @param vec input set of values
 * @return the mean of the input values
 */
double mean(const vector<double>& vec);

/**
 * Standard deviation of a population.
 * @param vec input set of values
 * @return the standard deviation of the input values
 */
double stdev(const vector<double>& vec);

/**
 * Tha Jaccard similarity of a set of sets is the size of their intersection divided by the size of their union.
 * @param sets a vector of sets
 * @return Jaccard similiarity of the input sets. If the sets are all empty, 0 is returned.
 */
template <class T>
double jaccard_similarity(const std::vector<hash_set<T> >& sets) {
	long union_size = s_union(sets).size();
	if (union_size==0)
		return 0;
	long intersection_size = s_intersection(sets).size();
	return (double)intersection_size/union_size;
}

    /**
     * Set-based intersection, for a combination of sorted and unordered sets.
     * @param set1 a set of values
     * @param set2 a set of values
     * @return the intersection of the two input sets
     */
    template <class T>
    int intersection_size(const hash_set<T>& set1, const hash_set<T>& set2) {
        int common_elements = 0;
        if (set1.size()<set2.size()) {
            for (T el: set1) {
                if (set2.count(el)>0)
                    common_elements++;
            }
        }
        else {
            for (T el: set2) {
                if (set1.count(el)>0)
                    common_elements++;
            }
        }
        return common_elements;
    }
    
/**
 * Set-based intersection, for unordered sets.
 * @param sets a vector of sets
 * @return the intersection of the input sets
 */
template <class T>
hash_set<T> s_intersection(const std::vector<hash_set<T> >& sets) {
	hash_set<T> result;
	uint idx = 0; // index of the smallest set
	for (uint i=1; i<sets.size(); i++) {
		if (sets.at(i).size() < sets.at(idx).size()) {
			idx=i;
		}
	}
	for (T element: sets.at(idx)) {
		bool in_intersection = true;
		for (uint i=0; i<sets.size(); i++) {
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

/**
 * Set-based intersection, for sorted sets.
 * @param sets a vector of sets
 * @return the intersection of the input sets
 */
template <class T>
hash_set<T> s_intersection(const vector<sorted_set<T> >& sets) {
	// NOTE: it can be made more efficient exploiting sorting
	hash_set<T> result;
	uint idx = 0; // index of the smallest set
	for (uint i=1; i<sets.size(); i++) {
		if (sets.at(i).size() < sets.at(idx).size()) {
			idx=i;
		}
	}
	for (T element: sets.at(idx)) {
		bool in_intersection = true;
		for (uint i=0; i<sets.size(); i++) {
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


/**
 * Set-based intersection, for unordered sets.
 * @param set1 a set of values
 * @param set2 a set of values
 * @return the intersection of the two input sets
 */
template <class T>
hash_set<T> s_intersection(const hash_set<T>& set1, const hash_set<T>& set2) {
	vector<hash_set<T> > sets({set1,set2});
	return s_intersection(sets);
}

/**
 * Set-based intersection, for sorted sets.
 * @param set1 a set of values
 * @param set2 a set of values
 * @return the intersection of the two input sets
 */
template <class T>
hash_set<T> s_intersection(const sorted_set<T>& set1, const sorted_set<T>& set2) {
	std::vector<sorted_set<T> > sets({set1,set2});
	return s_intersection(sets);
}


/**
 * Set-based intersection, for a combination of sorted and unordered sets.
 * @param set1 a set of values
 * @param set2 a set of values
 * @return the intersection of the two input sets
 */
template <class T>
hash_set<T> s_intersection(const sorted_set<T>& set1, const hash_set<T>& set2) {
	hash_set<T> result;
	for (T element: set1) {
		if (set2.count(element)>0) {
			result.insert(element);
		}
	}
	return result;
}

/**
 * Set-based intersection, for a combination of sorted and unordered sets.
 * @param set1 a set of values
 * @param set2 a set of values
 * @return the intersection of the two input sets
 */
template <class T>
hash_set<T> s_intersection(const hash_set<T>& set1, const sorted_set<T>& set2) {
	hash_set<T> result;
	for (T element: set2) {
		if (set1.count(element)>0) {
			result.insert(element);
		}
	}
	return result;
}


/**
 * Set-based union, for unordered sets.
 * @param sets a vector of sets
 * @return the union of the input sets
 */
template <class T>
hash_set<T> s_union(const vector<hash_set<T> >& sets) {
	hash_set<T> result;
	for (hash_set<T> S: sets) {
		result.insert(S.begin(), S.end());
	}
	return result;
}

/**
 * Set-based union, for sorted sets.
 * @param sets a vector of sets
 * @return the union of the input sets
 */
template <class T>
hash_set<T> s_union(const vector<sorted_set<T> >& sets) {
	hash_set<T> result;
	for (sorted_set<T> S: sets) {
		result.insert(S.begin(), S.end());
	}
	return result;
}

/**
 * Set-based union, for two sets.
 * @param set1 a set of values
 * @param set2 a set of values
 * @return the union of the two input sets
 */
template <class T>
hash_set<T> s_union(const hash_set<T>& set1, const hash_set<T>& set2) {
	hash_set<T> result;
	result.insert(set1.begin(), set1.end());
	result.insert(set2.begin(), set2.end());
	return result;
}

/**
 * Set-based union, for two sorted sets.
 * @param set1 a set of values
 * @param set2 a set of values
 * @return the union of the two input sets
 */
template <class T>
hash_set<T> s_union(const sorted_set<T>& set1, const sorted_set<T>& set2) {
	hash_set<T> result;
	result.insert(set1.begin(), set1.end());
	result.insert(set2.begin(), set2.end());
	return result;
}

/**
 * Converts a number to a string representation of it
 * @param number value to be converted
 * @return a string representation of the input
 */
template <typename T> std::string to_string (T number) {
	std::ostringstream ss;
    ss << number;
    return ss.str();
}

/**
 * Converts a string representation of a floating point number into its numeric value.
 * @param double_as_string a string representing a floating point number
 * @return a numerical value corresponding to the input
 */
double to_double(const std::string& double_as_string);

/**
 * Converts a string representation of a number into its numeric value.
 * @param int_as_string a string representing an integer
 * @return a numerical value corresponding to the input
 */
int to_int(const std::string& int_as_string);

/**
 * Converts a container to a string representation of it
 * @param first pointer to the first element in the container
 * @param last pointer to the last element in the container
 * @return a string representation of the container
 */
template <class InputIterator> std::string set_to_string(InputIterator first, InputIterator last) {
	std::ostringstream ss;
    ss << "( ";
    for (; first!=last; ++first)
    	ss << (*first) << " ";
    ss << ")";
    return ss.str();
}

/**
 * Converts a vector to a string representation of it
 * @param vec the vector
 * @return a string representation of the vector
 */
template <typename T>  std::string to_string(vector<T> vec) {
	std::ostringstream ss;
    ss << "( ";
    for (T el: vec)
    	ss << el << " ";
    ss << ")";
    return ss.str();
}

/**
 * Converts a set into a string representation of it
 * @param set the input set
 * @return a string representation of the set
 */
template <typename T>  std::string to_string(hash_set<T> set) {
	std::ostringstream ss;
    ss << "( ";
    for (T el: set)
    	ss << el << " ";
    ss << ")";
    return ss.str();
}

/**
 * Converts a set into a string representation of it
 * @param set the input set
 * @return a string representation of the set
 */
template <typename T>  std::string to_string(sorted_set<T> set) {
	std::ostringstream ss;
    ss << "( ";
    for (T el: set)
    	ss << el << " ";
    ss << ")";
    return ss.str();
}


}

#endif /* MLNET_MATH_H_ */
