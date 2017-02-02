/*
 * CSVReader.cpp
 *
 */

#include "utils.h"
#include "exceptions.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

namespace mlnet {

CSVReader::CSVReader() {
	next = "";
	has_next = true;
	remove_trailing_spaces = false;
	row_number = 0;
	field_separator = ',';
}

CSVReader::~CSVReader() {}

void CSVReader::open(const std::string& path) {
	infile.open(path.data());
	if (infile.fail()) throw FileNotFoundException(path);
	do {(getline(infile, next))?has_next=true:has_next=false;} while (next=="" && has_next);
	row_number = 0;
}

bool CSVReader::hasNext() const {
	return has_next;
}

void CSVReader::trimFields(bool value) {
	remove_trailing_spaces = value;
}

/*void CSVReader::isRowEmpty() {
	return is_empty;
}*/

std::vector<std::string> CSVReader::getNext() {
	row_number++;
	std::istringstream line(next);
	std::vector<std::string> record;

	while (line) {
		std::string field;
		if (!getline(line, field, field_separator)) break;
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


int CSVReader::rowNum() const {
	return row_number;
}

void CSVReader::setFieldSeparator(char separator) {
	field_separator = separator;
}

}
