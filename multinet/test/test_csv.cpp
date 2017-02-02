/*
 * Unit testing: csv.h
 */

#include "test.h"
#include "utils.h"

using namespace mlnet;

void test_csv() {

	test_begin("CSV");

	std::cout << "Reading a CSV file...";
	CSVReader csv;
	csv.trimFields(true);
	csv.setFieldSeparator(',');
	csv.open("test/test.csv");
	std::cout << "done!" << std::endl;
	vector<string> v = csv.getNext();
	std::cout << "Testing CSV settings...";
	if (v[0]=="V1 ") throw FailedUnitTestException("Trailing spaces not removed");
	csv.setFieldSeparator(',');
	v = csv.getNext();
	if (v.size()>2) throw FailedUnitTestException("Read too many fields");
	v = csv.getNext();
	if (csv.hasNext())  throw FailedUnitTestException("Empty line not skipped");
	std::cout << "done!" << std::endl;

	test_end("CSV");
}
