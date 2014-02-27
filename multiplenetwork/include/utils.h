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

#include <string>
#include <fstream>
#include <vector>
#include <set>

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
	* Returns a random integral number in the range [0,max[ using an
	* approximately uniform probability distribution.
	* @param max
	* @param k
	*/
	std::set<unsigned long> getKRandom(unsigned long max, unsigned int k);

	/* not implemented
	template <class T>
	std::set<T> getKRandom(std::set<T>, unsigned int k);
	 */

	/**
	 * Random test: returns TRUE with probability probability.
	 */
	bool test(double probability);

private:
	/**
	 * This class keeps an array of long values called options (from 0 to options.size()-1)
	 * used to return random numbers. When a set of numbers is requested in the interval
	 * [0,max[, the array is resized accordingly if necessary.
	 */
	void resizeOptions(unsigned long max);

};
#endif /* MULTIPLENETWORK_UTILS_H_ */
