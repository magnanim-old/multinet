/*
 * Unit testing for module: units
 *
 */

#include "test.h"
#include "utils.h"
#include <unordered_set>
#include <vector>
#include <iostream>

using namespace mlnet;

void test_utils() {

	test_begin("UTILITY FUNCTIONS AND CLASSES");

	test_begin("Math");
	std::cout << "Testing set functions...";
	std::unordered_set<int> S1({1,2,3,4,5});
	std::unordered_set<int> S2({3,4,5,6,7});
	std::unordered_set<int> S3({1,3,5,8});
	std::vector<std::unordered_set<int> > sets({S1,S2,S3});
	if (s_intersection(sets).size()!=2) throw FailedUnitTestException("Wrong set intersection");
	if (s_intersection(S1,S2).size()!=3) throw FailedUnitTestException("Wrong set intersection");
	if (s_union(sets).size()!=8) throw FailedUnitTestException("Wrong set union");
	if (s_union(S1,S2).size()!=7) throw FailedUnitTestException("Wrong set union");
	std::cout << "done!" << std::endl;
	test_end("Math");

	test_end("UTILITY FUNCTIONS AND CLASSES");
}


