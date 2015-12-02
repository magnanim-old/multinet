/**
 * counter.h
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 1.0
 * 
 * A commodity class to count the number of occurrences of a value or a pair of values.
 */

#ifndef MLNET_COUNTER_H_
#define MLNET_COUNTER_H_

#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <memory>
#include "random.h"
#include "exceptions.h"
#include <cmath>

namespace mlnet {

/**********************************************************************/
/** Counter ***********************************************************/
/**********************************************************************/

/**
 * A Counter for a single value
 */
template <class T>
class Counter {
private:
	/** A map where the count is kept */
    std::unordered_map<T,long> values;

public:
    /**
     * Constructor.
     */
    Counter();

    /**
     * Destructor.
     */
    ~Counter();

    /**
     * This function is used to increase the count of T.
     * @param val value whose count should be incremented
     */
    void inc(T val);


    /**
     * This function is used to set a given value for the count of T.
     * @param val value whose count should be incremented
     */
    void set(T val, long num);

    /**
     * @param val value whose count should be returned
     * @return the count of T
     */
    long count(T val) const;

    /**
     * @return the value with the highest count (in case of tie, any maximal value can be returned)
     */
    T max() const;

    std::unordered_map<T,long>& map();
};

template <class T>
Counter<T>::Counter() {}

template <class T>
Counter<T>::~Counter() {
}

template <class T>
void Counter<T>::inc(T val) {
	if (values.count(val)==0) {
		values[val] = 0;
	}
	values[val]++;
}

template <class T>
void Counter<T>::set(T val, long num) {
	values[val] = num;
}

template <class T>
long Counter<T>::count(T val) const {
	if (values.count(val)==0) {
		return 0;
	}
	else return values.at(val);
}

template <class T>
T Counter<T>::max() const {
	long max = 0;
	T max_value;
	for (auto pair: values) {
		if (pair.second>max) {
			max_value = pair.first;
			max = pair.second;
		}
	}
	if (max==0)
		return NULL;
	else return max_value;
}

template <class T>
std::unordered_map<T, long>& Counter<T>::map() {
	return values;
}

/**
 * A Counter for a pair of values
 */
template <class T1, class T2>
class PairCounter {
private:
	/** A map where the count is kept */
    std::unordered_map<T1, std::unordered_map<T2,long> > values;

public:
    /**
     * Constructor.
     */
    PairCounter();

    /**
     * Destructor.
     */
    ~PairCounter();

    /**
     * This function is used to increase the count of the pair T1,T2.
     * @param val value whose count should be incremented
     */
    void inc(T1 val1, T2 val2);

    /**
     * This function is used to set a given value for the count of the pair T1,T2.
     * @param val value whose count should be incremented
     */
    void set(T1 val1, T2 val2, long num);

    /**
     * @param val value whose count should be returned
     * @return count of the pair T1,T2
     */
    long count(T1 val1, T2 val2) const;

    std::unordered_map<T1, std::unordered_map<T2,long> >& map();
};

template <class T1, class T2>
PairCounter<T1, T2>::PairCounter() {}

template <class T1, class T2>
PairCounter<T1, T2>::~PairCounter() {
}

template <class T1, class T2>
void PairCounter<T1, T2>::inc(T1 val1, T2 val2) {
	if (values.count(val1)==0 || values.at(val1).count(val2)==0) {
		values[val1][val2] = 0;
	}
	values[val1][val2]++;
}

template <class T1, class T2>
void PairCounter<T1, T2>::set(T1 val1, T2 val2, long num) {
	values[val1][val2] = num;
}

template <class T1, class T2>
long PairCounter<T1,T2>::count(T1 val1, T2 val2) const {
	if (values.count(val1)==0 || values.at(val1).count(val2)==0) {
		return 0;
	}
	else return values.at(val1).at(val2);
}

template <class T1, class T2>
std::unordered_map<T1, std::unordered_map<T2,long> >& PairCounter<T1,T2>::map() {
	return values;
}

} // namespace mlnet

#endif /* MLNET_COUNTER_H_ */
