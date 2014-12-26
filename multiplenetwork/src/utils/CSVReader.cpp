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
	next = "";
	has_next = true;
	remove_trailing_spaces = false;
}

CSVReader::~CSVReader() {
	// TODO Auto-generated destructor stub
}

void CSVReader::open(string path) {
	infile.open(path.data());
	if (infile.fail()) throw FileNotFoundException(path);
	do {(getline(infile, next))?has_next=true:has_next=false;} while (next=="" && has_next);
	row_number = 0;
}

bool CSVReader::hasNext() {
	return has_next;
}

void CSVReader::trimFields(bool value) {
	remove_trailing_spaces = value;
}

/*void CSVReader::isRowEmpty() {
	return is_empty;
}*/

vector<string> CSVReader::getNext() {
	row_number++;
	istringstream line(next);
	vector<string> record;

	while (line) {
		string field;
		if (!getline(line, field, ',')) break;
		if (remove_trailing_spaces) {
			field.erase(field.find_last_not_of(" \n\r\t")+1);
			field.erase(0,field.find_first_not_of(" \n\r\t"));
		}
		record.push_back(field);
	}

	// prepare next line for next call
	do {(getline(infile, next))?has_next=true:has_next=false;} while (next=="" && has_next);

	return record;
}


int CSVReader::rowNum() {
	return row_number;
}
