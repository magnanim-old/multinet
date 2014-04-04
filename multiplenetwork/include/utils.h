/*
 * utils.h
 *
 * Created on: Feb 8, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 *
 * Contains:
 * - logging functions
 * - string manipulation functions
 */

#ifndef MULTIPLENETWORK_UTILS_H_
#define MULTIPLENETWORK_UTILS_H_

#include "datastructures.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <random>

typedef int verbosity;

const int VERBOSITY=2;
const int V_OFF=0;
const int V_BASIC=1;
const int V_VERBOSE=2;
const int V_DEBUG=3;

void log(std::string s);
void log(std::string s, bool new_line);
void log(std::string s, verbosity v);
void log(std::string s, verbosity v, bool new_line);
void warn(std::string s);
void err(std::string s);

template <class T> void print(std::set<T>& input) {
	typename std::set<T>::iterator it;
	for (it=input.begin(); it!=input.end(); ++it) {
		if (it!=input.begin()) std::cout << ",";
		std::cout << (*it);
	}
	std::cout << std::endl;
}

/***********************************/
/** String manipulation functions **/
/***********************************/
//std::string n2s (long number);

/***********************************/
/** IO                            **/
/***********************************/
class CSVReader {
private:
	std::ifstream infile;
	std::string next;
	bool has_next;

public:
	CSVReader();
	virtual ~CSVReader();

	void open(std::string path);
	bool hasNext();
	std::vector<std::string> getNext();
};


/***********************************/
/** Random                        **/
/***********************************/
class Random {
	std::vector<long> options;

public:
	Random();
	~Random();

	/**
	 * Returns a random integral number in the range [0,max[ using an
	 * approximately uniform probability distribution.
	 * @param max
	 */
	long getRandom(unsigned long max);

	/**
	 * Returns a random double number in the range [0,1] using an
	 * approximately uniform probability distribution. Not all double values
	 * between 0 and 1 can be returned.
	 */
	double getRandomDouble();

	/**
	* Returns K random integral numbers in the range [0,max[ using an
	* approximately uniform probability distribution.
	* @param max
	* @param k
	*/
	std::set<unsigned long> getKRandom(unsigned long max, unsigned int k);

	template <class T>
	void getKElements(std::set<T>& input, std::set<T>& output, unsigned int k) {

		  std::uniform_int_distribution<int> distribution(0,input.size());
		  // TODO add check on size
		  while (output.size()<k) {
		    int offset = distribution(generator);
		    typename std::set<T>::iterator it;
		    int i = 0;
		    for (it=input.begin(); i<offset; ++it) i++;
		    output.insert(*it);
		  }
	}

	template <class T>
	T getElement(std::set<T>& input);

	/**
	 * Random test: returns TRUE with probability probability.
	 */
	bool test(double probability);

private:
	std::default_random_engine generator;
	/**
	 * This class keeps an array of long values called options (from 0 to options.size()-1)
	 * used to return random numbers. When a set of numbers is requested in the interval
	 * [0,max[, the array is resized accordingly if necessary.
	 */
	void resizeOptions(unsigned long max);

};

#endif /* MULTIPLENETWORK_UTILS_H_ */
