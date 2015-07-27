/*
 * utils.h
 *
 * Contains:
 * - logging functions
 * - basic IO (csv file reading)
 * - random functions
 */

#ifndef MULTIPLENETWORK_RANDOM_H_
#define MULTIPLENETWORK_RANDOM_H_

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <random>

/***********************************/
/** Random                        **/
/***********************************/
namespace random_utils {
	/**
	 * Returns a random integral number in the range [0,max[ using an
	 * approximately uniform probability distribution.
	 * @param max
	 * @throws OperationNotSupprtedException if the range is larger than the number of random numbers that can be returned by the system
	 */
	int getRandomInt(int max);

	/**
	 * Returns a random integral number in the range [0,max[ using an
	 * approximately uniform probability distribution.
	 * @param max
	 * @throws OperationNotSupprtedException if the range is larger than the number of random numbers that can be returned by the system
	 */
	long getRandomLong(long max);

	/**
	 * Returns a random double number in the range [0,1[ using an
	 * approximately uniform probability distribution.
	 */
	double drand();

	int random_level(int MAX_LEVEL, double P);

	/**
	* Returns K random integral numbers in the range [0,max[ using an
	* approximately uniform probability distribution.
	* @param max
	* @param k
	*/
	std::set<unsigned int> getKRandom(unsigned int max, unsigned int k);

	/**
	* Returns K random elements from the input set
	* chosen with an approximately uniform probability distribution.
	* @param input
	* @param k
	*
	template <class T>
	std::set<T> getKElements(const std::set<T>& input, unsigned int k) {
		std::set<T> output;
		std::set<unsigned int> choices = getKRandom(input.size(),k);
		std::vector<T> v(input.begin(),input.end());
		for (int choice: choices) {
		    output.insert(v.at(choice));
		}
		return output;
	}

	template <class T>
	T getElement(const std::set<T>& input) {
		return *getKElements(input, 1).begin();
	}*/

	/**
	 * Random test: returns TRUE with probability probability.
	 */
	bool test(double probability);
}

#endif /* MULTIPLENETWORK_UTILS_H_ */
