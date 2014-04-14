/*
 * test.cpp
 *
 * Created on: Jun 11, 2013
 * Author: magnanim
 */


#include <set>
#include <map>
#include <array>
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

	cout << "Testing random integer in [0,10[, 1000 iterations.." << endl;
	std::array<int,10> occurrences;
	occurrences.fill(0);
	for (int i=0; i<1000; i++) {
		int value = r.getRandomInt(10);
		if (value<0 || value>=10) throw FailedUnitTestException("Random number sampled out of range [0,10[");
		occurrences[value]++;
	}
	cout << "done! Hits per value:";
	for (int& x : occurrences) { std::cout << ' ' << x; }
	std::cout << std::endl;

	cout << "Testing random double in [0,1[, 1000 iterations.." << endl;
	for (int i=0; i<1000; i++) {
		double value = r.getRandomDouble();
		if (value<0 || value>=1) throw FailedUnitTestException("Random number sampled out of range [0,1[");
	}
	cout << "done! 3 examples:";
	for (int i=0; i<3; i++) {
		cout << ' ' << r.getRandomDouble();
	}
	std::cout << std::endl;

	cout << "Testing random choice, p=.75, 1000 iterations.." << endl;
	std::array<int,2> outcome;
	outcome.fill(0);
	for (int i=0; i<1000; i++) {
		bool value = r.test(.75);
		if (value) outcome[0]++;
		else outcome[1]++;
	}
	cout << "done! Positive and negative outcomes:";
	for (int& x : outcome) { std::cout << ' ' << x; }
	std::cout << std::endl;

	/*
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
