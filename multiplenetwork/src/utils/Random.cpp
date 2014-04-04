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

using namespace std;

Random::Random() {
	generator.seed(time(0));
	srand(time(0));
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

long Random::getRandom(unsigned long max) {
	unsigned long res = (unsigned long)((double)rand()/RAND_MAX*max); // [0,max[
	return res==max?max-1:res;
}

double Random::getRandomDouble() {
	double res = (double)rand()/RAND_MAX;
	std::cout << res << std::endl;
	return res; // [0,1]
}

set<unsigned long> Random::getKRandom(unsigned long max, unsigned int k) {
	resizeOptions(max);
	set<unsigned long> res;
	for (unsigned int i=0; i<k; i++) {
		long idx = getRandom(options.size()-i);
		res.insert(options[idx]);
		swap(options[idx],options[options.size()-i-1]);
	}
	return res;
}

bool Random::test(double probability) {
	std::bernoulli_distribution distribution(probability);
	return distribution(generator);//getRandomDouble()<=probability;
}

void Random::resizeOptions(unsigned long max) {
	if (max<options.size()) {
		options.clear();
	}
	while (max>options.size()) {
			options.push_back(options.size());
	}
}
