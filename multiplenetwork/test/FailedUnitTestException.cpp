/*
 * FileNotFoundException.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: magnanim
 */

#include "test.h"

using namespace std;

FailedUnitTestException::FailedUnitTestException(string value) {
	FailedUnitTestException::message = message;
}
FailedUnitTestException::~FailedUnitTestException() throw (){
	// TODO Auto-generated destructor stub
}
const char* FailedUnitTestException::what() const throw()
{
    return (string("[FAIL] ") + message).data();
}
