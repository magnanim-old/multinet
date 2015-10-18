/*
 * Random.cpp
 *
 *  Created on: 14/ago/2013
 *      Author: stud10
 */

#include "utils.h"
#include "exceptions.h"
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <time.h>
//#include <random>
//#include <chrono>

using namespace std;

namespace mlnet {
//Random::Random() {
	/*
	// C++11 version:
	std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::duration dtn = time.time_since_epoch();
	generator.seed(dtn.count());
	*/

//	srand(time(NULL));
//}

//Random::~Random() {}

int getRandomInt(int max) {
	if (max>RAND_MAX) throw OperationNotSupportedException("Requested random value " + to_string(max) + "larger than RAND_MAX constant");
	return rand() % max;
	/*
	// C++11 version:
	std::uniform_int_distribution<int> distribution(0,max-1);
	return distribution(generator);
	*/
}

long getRandomLong(long max) {
	if (max>RAND_MAX) throw OperationNotSupportedException("Requested random value " + to_string(max) + "larger than RAND_MAX constant");
	return rand() % max;
	/*
	// C++11 version:
	std::uniform_int_distribution<long> distribution(0,max-1);
	return distribution(generator);
	*/
}

double drand() {
	return double(rand()) / RAND_MAX;
	/*
	 // C++11 version:
	  std::uniform_real_distribution<double> distribution(0,1);
	  return distribution(generator); // [0,1[
	  */
}

int random_level(int MAX_LEVEL, double P) {
    static bool first = true;

    if (first) {
        srand( (unsigned)time( NULL ) );
        first = false;
    }

    int lvl = (int)(std::log(drand())/std::log(1.0-P));
    return lvl < MAX_LEVEL ? lvl : MAX_LEVEL;
}

set<unsigned int> getKRandom(unsigned int max, unsigned int k) {
	set<unsigned int> res;
	while (res.size()<k)
		res.insert(getRandomInt(max));
	/*
	 // alternative version
	resizeOptions(max);
	set<unsigned long> res;
	for (unsigned int i=0; i<k; i++) {
		long idx = getRandomInt(options.size()-i);
		res.insert(options[idx]);
		swap(options[idx],options[options.size()-i-1]);
	}
	*/
	return res;
}

bool test(double probability) {
	return (drand()<probability);
	/*
	// C++11 version:
	std::bernoulli_distribution distribution(probability);
	return distribution(generator);
	*/
}

int test(const std::vector<double>& options) {
	double prob_failing_previous_tests=1;
	for (int idx=0; idx<options.size()-1; idx++) {
		double adjusted_prob = options.at(idx)/prob_failing_previous_tests;
		if (test(adjusted_prob))
			return idx;
		prob_failing_previous_tests *= (1-adjusted_prob);
	}
	return options.size()-1;
}

/*
void Random::resizeOptions(unsigned long max) {
	if (max<options.size()) {
		options.clear();
	}
	while (max>options.size()) {
			options.push_back(options.size());
	}
}*/

}
