/*
 * CSVReader.cpp
 *
 *  Created on: Jun 27, 2013
 *      Author: magnanim
 */

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include "utils.h"
#include "exceptions.h"

using namespace std;

CSVReader::CSVReader() {
	// TODO Exceptions
}

CSVReader::~CSVReader() {
	// TODO Auto-generated destructor stub
}

void CSVReader::open(string path) {
	infile.open(path.data());
	if (infile.fail()) throw FileNotFoundException(path);
	(getline(infile, next))?has_next=true:has_next=false;
}

bool CSVReader::hasNext() {
	return has_next;
}

vector<string> CSVReader::getNext() {
	istringstream line(next);
	vector<string> record;

	while (line) {
		string field;
		if (!getline(line, field, ',')) break;
		record.push_back(field);
	}

	// prepare next line for next call
	(getline(infile, next))?has_next=true:has_next=false;

	return record;
}
