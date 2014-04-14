/*
 * Random.cpp
 *
 *  Created on: 14/ago/2013
 *      Author: stud10
 */

#include "utils.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>

using namespace std;

Random::Random() {
	std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::duration dtn = time.time_since_epoch();

	generator.seed(dtn.count());
}

Random::~Random() {
	// TODO Auto-generated destructor stub
}

/*template <class T>
void Random::getKElements(std::set<T>& input, std::set<T>& output, unsigned int k)  {
	  std::default_random_engine generator;
	  std::uniform_int_distribution<int> distribution(0,input.size());

	  // TODO add check on size
	  while (output.size()<k) {
	    int offset = distribution(generator);
	    output.insert(input.begin()+offset);
	  }
}*/

int Random::getRandomInt(int max) {
	  std::uniform_int_distribution<int> distribution(0,max-1);
	  return distribution(generator);
}

long Random::getRandomLong(long max) {
	  std::uniform_int_distribution<long> distribution(0,max-1);
	  return distribution(generator);
}

double Random::getRandomDouble() {
	  std::uniform_real_distribution<double> distribution(0,1);
	  return distribution(generator); // [0,1[
}

set<unsigned long> Random::getKRandom(unsigned long max, unsigned int k) {
	resizeOptions(max);
	set<unsigned long> res;
	for (unsigned int i=0; i<k; i++) {
		long idx = getRandomInt(options.size()-i);
		res.insert(options[idx]);
		swap(options[idx],options[options.size()-i-1]);
	}
	return res;
}

bool Random::test(double probability) {
	std::bernoulli_distribution distribution(probability);
	return distribution(generator);
}

void Random::resizeOptions(unsigned long max) {
	if (max<options.size()) {
		options.clear();
	}
	while (max>options.size()) {
			options.push_back(options.size());
	}
}
