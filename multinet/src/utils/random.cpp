#include "utils.h"
#include "exceptions.h"
#include <algorithm>

namespace mlnet {
//Random::Random() {

	// C++11 version: not supported by some systems yet
	//std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
	//std::chrono::high_resolution_clock::duration dtn = time.time_since_epoch();
	//auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
	//mt19937 generator;
	//generator.seed(seed);


//	srand(time(NULL));
//}

	std::mt19937 & get_random_engine() {
		static std::mt19937 engine;
		static bool seed = true;
		if (seed) {
			//std::cout << " SEEEEEEEEEEEEEEEEEEEEEEEEED " << std::endl;
			engine.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
			seed = false;
		}
		return engine;
	}

//Random::~Random() {}

int getRandomInt(int max) {
	//if (max>RAND_MAX) throw OperationNotSupportedException("Requested random value " + to_string(max) + "larger than RAND_MAX constant");
	//return rand() % max;

	// C++11 version:
	std::uniform_int_distribution<int> distribution(0,max-1);
	return distribution(get_random_engine());

}

long getRandomLong(long max) {
	//if (max>RAND_MAX) throw OperationNotSupportedException("Requested random value " + to_string(max) + "larger than RAND_MAX constant");
	//return rand() % max;

	// C++11 version:
	std::uniform_int_distribution<long> distribution(0,max-1);
	return distribution(get_random_engine());

}

double drand() {
	//return double(rand()) / RAND_MAX;

	 // C++11 version:
	 std::uniform_real_distribution<double> distribution(0,1);
	 return distribution(get_random_engine()); // [0,1[
}

int random_level(int MAX_LEVEL, double P) {
    double r = drand();
    if (r==0) r=1; // avoid taking logarithm of 0
    double num = std::log(r);
    double denum = std::log(1.0-P);
    int lvl = (int)(num/denum);
    return lvl < MAX_LEVEL ? lvl : MAX_LEVEL;
}

std::set<long> getKRandom(long max, uint k) {
	if (max<k) throw OperationNotSupportedException("Only " + to_string(max) + " values available, requested " + to_string(k));
	std::set<long> res;
	while (res.size()<k)
		res.insert(getRandomInt(max));
	return res;
}

bool test(double probability) {
	//return (drand()<probability);

	// C++11 version:
	std::bernoulli_distribution distribution(probability);
	return distribution(get_random_engine());
}

int test(const std::vector<double>& options) {
	// For efficiency reasons, we do not check if the values sum to 1
	double prob_failing_previous_tests=1;
	for (uint idx=0; idx<options.size()-1; idx++) {
		double adjusted_prob = options.at(idx)/prob_failing_previous_tests;
		if (test(adjusted_prob))
			return idx;
		prob_failing_previous_tests *= (1-adjusted_prob);
	}
	// In practice, the last value of the input is assumed to be 1 minus the sum of the previous values
	return options.size()-1;
}

int test(const std::vector<std::vector<double> >& options, int row_num) {
	// For efficiency reasons, we do not check if the values sum to 1
	double prob_failing_previous_tests=1;
	for (uint idx=0; idx<options.at(row_num).size()-1; idx++) {
		double adjusted_prob = options.at(row_num).at(idx)/prob_failing_previous_tests;
		if (test(adjusted_prob))
			return idx;
		prob_failing_previous_tests *= (1-adjusted_prob);
	}
	// In practice, the last value of the input is assumed to be 1 minus the sum of the previous values
	return options.at(row_num).size()-1;
}

}
