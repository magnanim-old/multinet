/*
 * Unit testing: propertymatrix.h
 */

#include "test.h"
#include "utils.h"

using namespace mlnet;

void test_propertymatrix() {

	test_begin("Property matrix");

	std::cout << "Creating a boolean property matrix...";
	property_matrix<int,int,bool> P(4,2,true);
	std::cout << " done!" << std::endl;
	std::cout << "Setting values...";
	P.set(0,0,false);
	P.set(2,1,false);
	P.set(3,0,false);
	P.set(3,1,false);
	std::cout << " done!" << std::endl;
	std::cout << "Retrieving values...";
	if (P.get(0,0).val) throw FailedUnitTestException("Wrong value for field 0,0");
	if (P.get(2,1).val) throw FailedUnitTestException("Wrong value for field 2,1");
	if (!P.get(0,1).val) throw FailedUnitTestException("Wrong default value for field 0,1");
	std::cout << " done!" << std::endl;
	std::cout << "Listing headers...";
	if (P.structures().size()!=3) throw FailedUnitTestException("Wrong number of modified structures");
	if (P.contexts().size()!=2) throw FailedUnitTestException("Wrong number of modified contexts");
	std::cout << " done!" << std::endl;
	std::cout << "Testing functions...";
	if (russell_rao(P,0,1)!=1.0/4) throw FailedUnitTestException("Wrong function: russell_rao");
	if (coverage(P,0,1)!=1.0/2) throw FailedUnitTestException("Wrong function: coverage");
	if (jaccard(P,0,1)!=1.0/3) throw FailedUnitTestException("Wrong function: jaccard");
	if (kulczynski2(P,0,1)!=1.0/2) throw FailedUnitTestException("Wrong function: kulczynski2");
	if (simple_matching(P,0,1)!=2.0/4) throw FailedUnitTestException("Wrong function: simple_matching");
	std::cout << " done!" << std::endl;

	std::cout << "Creating a numeric property matrix...";
	property_matrix<int,int,double> PI(4,2,0);
	std::cout << " done!" << std::endl;
	std::cout << "Setting values...";
	PI.set(1,0,1);
	PI.set(1,1,1);
	PI.set(2,0,1);
	PI.set(2,1,3);
	PI.set(3,0,2);
	PI.set(3,1,4);
	std::cout << " done!" << std::endl;
    /*
	std::cout << "Testing functions...";
    std::cout << "mean1: " << mean(PI,0) << std::endl;
    std::cout << "mean2: " << mean(PI,1) << std::endl;
    std::cout << "diff: " << relative_difference(mean,PI,0,1) << std::endl;
    std::cout << "D: " << dissimilarity_index(PI,0,1,2) << std::endl;
    std::cout << "KL: " << KL_divergence(PI,0,1,2) << std::endl;
    std::cout << "J: " << jeffrey_divergence(PI,0,1,2) << std::endl;
    // etc.
	if (std::abs(pearson(PI,0,1)-.8944272)>0.001) throw FailedUnitTestException("Wrong function: pearson");
	PI.rankify();
	if (std::abs(pearson(PI,0,1)-.9486833)>0.001) throw FailedUnitTestException("Wrong function: rank correlation");
    */
	std::cout << " done!" << std::endl;

	test_end("Property matrix");
}
