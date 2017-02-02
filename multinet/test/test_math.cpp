/*
 * Unit testing: math.h
 */

#include "test.h"
#include "utils.h"
#include <iostream>

using namespace mlnet;

void test_math() {

	test_begin("Math");
	std::cout << "Testing set functions...";
	hash_set<int> HS1({1,2,3,4,5});
	hash_set<int> HS2({3,4,5,6,7});
	hash_set<int> HS3({1,3,5,8});
	vector<hash_set<int> > sets({HS1,HS2,HS3});
	if (s_intersection(sets).size()!=2) throw FailedUnitTestException("Wrong set intersection");
	if (s_intersection(HS1,HS2).size()!=3) throw FailedUnitTestException("Wrong set intersection");
	if (s_union(sets).size()!=8) throw FailedUnitTestException("Wrong set union");
	if (s_union(HS1,HS2).size()!=7) throw FailedUnitTestException("Wrong set union");
	sorted_set<int> SS1({1,2,3,4,5});
	sorted_set<int> SS2({3,4,5,6,7});
	sorted_set<int> SS3({1,3,5,8});
	vector<sorted_set<int> > s_sets({SS1,SS2,SS3});
	if (s_intersection(s_sets).size()!=2) throw FailedUnitTestException("Wrong set intersection");
	if (s_intersection(SS1,SS2).size()!=3) throw FailedUnitTestException("Wrong set intersection");
	if (s_union(s_sets).size()!=8) throw FailedUnitTestException("Wrong set union");
	if (s_union(SS1,SS2).size()!=7) throw FailedUnitTestException("Wrong set union");
	std::cout << "done!" << std::endl;

	std::cout << "Testing mathematical/statistical functions...";
	vector<double> vec(HS1.begin(),HS1.end());
	if (mean(vec)!=3) throw FailedUnitTestException("Wrong mean");
	if (std::abs(stdev(vec)-std::sqrt(2))>.001) throw FailedUnitTestException("Wrong standard deviation");
	std::cout << "done!" << std::endl;

	std::cout << "Testing conversion number -> string -> number...";
	if (to_double(to_string(to_double("2.34")))!=2.34) throw FailedUnitTestException("Wrong number conversion");
	std::cout << "done!" << std::endl;

	std::cout << "Testing conversion set -> string...";
	std::cout << set_to_string(SS1.begin(),SS1.end());
	std::cout << " done!" << std::endl;

	test_end("Math");

}


