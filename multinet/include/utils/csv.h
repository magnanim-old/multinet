/*
 * csv.h
 *
 * csv file reader.
 */

#ifndef MLNET_CSV_H_
#define MLNET_CSV_H_

#include <unordered_set>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>

namespace mlnet {

/**
 * Utility class to read a CSV file.
 */
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

	/**
	 * Reads a csv file.
	 * @param path csv file
	 */
	void open(const std::string& path);
    
    /**
     * Closes a csv file.
     */
    void close();
    
	/**
	 * Checks if the end of file has been reached.
	 * @return true if there is at least one row still to be read
	 */
	bool hasNext() const;

	/**
	 * Returns the next row from the file.
	 * Empty lines are automatically skipped.
	 * @return a vector of strings, each representing a field
	 */
	std::vector<std::string> getNext();

	/**
	 * @return the current row number.
	 */
	int rowNum() const;

	/**
	 * Controls whether trailing blank characters at the end of a field should be kept or removed.
	 * The default value is false.
	 * @param trim if true, the strings are trimmed
	 */
	void trimFields(bool trim);

	/**
	 * Sets the character used to separate fields.
	 * The default value is ','.
	 * @param separator a character separating strings
	 */
	void setFieldSeparator(char separator);
};

}

#endif /* MLNET_CSV_H_ */
