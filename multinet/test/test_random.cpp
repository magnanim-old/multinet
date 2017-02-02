/*
 * Unit testing: random.h
 */

#include "test.h"
#include "utils.h"

using namespace std;
using namespace mlnet;

void test_random() {

	test_begin("Random");

	cout << "Testing random integer in [0,10[, 1000 iterations...";
	std::array<int,10> occurrences;
	occurrences.fill(0);
	for (int i=0; i<1000; i++) {
		int value = getRandomInt(10);
		if (value<0 || value>=10) throw FailedUnitTestException("Random number sampled out of range [0,10[");
		occurrences[value]++;
	}
	cout << "done! Hits per value:";
	for (int& x : occurrences) { std::cout << ' ' << x; }
	std::cout << std::endl;

	cout << "Testing random double in [0,1[, 1000 iterations...";
	for (int i=0; i<1000; i++) {
		double value = drand();
		if (value<0 || value>=1) throw FailedUnitTestException("Random number sampled out of range [0,1[");
	}
	cout << "done! 3 examples:";
	for (int i=0; i<3; i++) {
		cout << ' ' << drand();
	}
	std::cout << std::endl;

	cout << "Testing random choice, p=.75, 1000 iterations...";
	std::array<int,2> outcome;
	outcome.fill(0);
	for (int i=0; i<1000; i++) {
		bool value = test(.75);
		if (value) outcome[0]++;
		else outcome[1]++;
	}
	cout << "done! Positive and negative outcomes:";
	for (int& x : outcome) { std::cout << ' ' << x; }
	std::cout << std::endl;

	cout << "Testing index selection, prob: 0:0.25, 1:0.25, 2:0.5, 1000 iterations...";
	std::vector<double> options;
	options.resize(3);
	options[0] = .25;
	options[1] = .25;
	options[2] = .50;
	std::array<int,3> hits;
	hits.fill(0);
	for (int i=0; i<1000; i++) {
		hits[test(options)]++;
	}
	cout << "done! Hits per value:";
	for (int& x : hits) { std::cout << ' ' << x; }
	std::cout << std::endl;

	test_end("Random");
}
