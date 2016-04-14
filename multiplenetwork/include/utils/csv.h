/*
 * csv.h
 *
 * csv file reader.
 */

#ifndef MLNET_CSV_H_
#define MLNET_CSV_H_

#include "exceptions.h"
#include <unordered_set>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>

namespace mlnet {

class CSVReader {
private:
	std::ifstream infile;
	std::string next;
	bool has_next;
	bool remove_trailing_spaces;
	int row_number;
	char field_separator;

public:
	CSVReader();
	virtual ~CSVReader();

	void open(std::string path);
	bool hasNext();
	std::vector<std::string> getNext();
	int rowNum();
	void trimFields(bool value);
	void setFieldSeparator(char separator);
};

}

#endif /* MLNET_CSV_H_ */
