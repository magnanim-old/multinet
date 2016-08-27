/*
 * Unit testing: counter.h
 */

#include "test.h"
#include "utils.h"

using namespace mlnet;

void test_counter() {

	test_begin("Counters");

	std::cout << "Testing simple counter...";
	Counter<int> c1;
	c1.set(1,4);
	c1.inc(1);
	c1.inc(2);
	if (c1.max()!=1)  throw FailedUnitTestException("Wrong counter operation: max");
	if (c1.count(3)!=0)  throw FailedUnitTestException("Wrong counter operation: default");
	if (c1.count(2)!=1)  throw FailedUnitTestException("Wrong counter operation: inc");
	if (c1.count(1)!=5)  throw FailedUnitTestException("Wrong counter operation: set");
	std::cout << "done!" << std::endl;


	std::cout << "Testing pair counter...";
	PairCounter<int,int> c2;
	c2.set(1,1,4);
	c2.inc(1,1);
	c2.inc(2,1);
	if (c2.count(3,1)!=0)  throw FailedUnitTestException("Wrong counter operation: default");
	if (c2.count(2,1)!=1)  throw FailedUnitTestException("Wrong counter operation: inc");
	if (c2.count(1,1)!=5)  throw FailedUnitTestException("Wrong counter operation: set");
	std::cout << "done!" << std::endl;


	std::cout << "Testing triplet counter...";
	TripletCounter<int,int,int> c3;
	c3.set(1,1,1,4);
	c3.inc(1,1,1);
	c3.inc(2,1,1);
	if (c3.count(3,1,1)!=0)  throw FailedUnitTestException("Wrong counter operation: default");
	if (c3.count(2,1,1)!=1)  throw FailedUnitTestException("Wrong counter operation: inc");
	if (c3.count(1,1,1)!=5)  throw FailedUnitTestException("Wrong counter operation: set");
	std::cout << "done!" << std::endl;

	test_end("Counters");
}
