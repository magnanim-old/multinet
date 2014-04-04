/*
 * test.cpp
 *
 * Created on: Jun 11, 2013
 * Author: magnanim
 */


#include <set>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "test.h"
#include "utils.h"
#include "exceptions.h"

using namespace std;

void testRandom() {

	// WORK IN PROGRESS

	set<unsigned long>::iterator it;
	map<unsigned long, int>::iterator itm;
	Random r;
	map<unsigned long, int> count;
	set<unsigned long> res;
	//unsigned long randomLong;
	//double randomDouble;

	vertex_id data[] = {1,2,3,4,6,7};
	std::set<vertex_id> input(data,data+6);
	std::set<vertex_id> output;
	r.getKElements(input,output,3);
	print(output);

	/*
	cout << "Testing random long [0,10[, 10000 iterations.." << endl;
	for (int i=0; i<10000; i++) {
		randomLong = r.getRandom(10);
		if (randomLong<0 || randomLong>=10) throw FailedUnitTestException("Random number sampled out of range [0,10[");
		count[randomLong]++;
	}
	cout << "done! Frequencies:" << endl;
	for (itm=count.begin(); itm!=count.end(); ++itm)
		    cout << itm->first << " => " << itm->second << endl;
	count.clear();

	cout << "Testing random double [0,1], 10000 iterations.." << endl;
	for (int i=0; i<10000; i++) {
		randomDouble = r.getRandomDouble();
		if (randomDouble<0 || randomDouble>1) throw FailedUnitTestException("Random number sampled out of range [0,1]");
	}
	cout << "done! Examples:" << endl;
	for (int i=0; i<10; i++) {
		cout << r.getRandomDouble() << endl;
	}

	cout << "Testing random set of values in [0,10[, 10000 iterations.." << endl;
	for (int i=0; i<10000; i++) {
		res = r.getKRandom(10,5);
			for (it=res.begin(); it!=res.end(); ++it) {
			if (*it<0 || *it>=10) throw FailedUnitTestException("Random number sampled out of range [0,10[");
			count[*it]++;
		}
	}
	std::cout << "done! Frequencies:\n";
	for (itm=count.begin(); itm!=count.end(); ++itm)
	    std::cout << itm->first << " => " << itm->second << '\n';
	count.clear();

	cout << "Testing random set of values in [0,15[, 10000 iterations.." << endl;
	for (int i=0; i<10000; i++) {
		res = r.getKRandom(15,5);
		for (it=res.begin(); it!=res.end(); ++it) {
			if (*it<0 || *it>=15) throw FailedUnitTestException("Random number sampled out of range [0,10[");
			count[*it]++;
		}
	}
	std::cout << "done! Frequencies:\n";
	for (itm=count.begin(); itm!=count.end(); ++itm)
		std::cout << itm->first << " => " << itm->second << '\n';
	count.clear();

	cout << "Testing random set of values in [0,8[, 10000 iterations.." << endl;
	for (int i=0; i<10000; i++) {
		res = r.getKRandom(8,5);
		for (it=res.begin(); it!=res.end(); ++it) {
			if (*it<0 || *it>=8) throw FailedUnitTestException("Random number sampled out of range [0,10[");
			count[*it]++;
		}
	}
	std::cout << "done! Frequencies:\n";
	for (itm=count.begin(); itm!=count.end(); ++itm)
	    std::cout << itm->first << " => " << itm->second << '\n';
	count.clear();
	*/
}
