/*
 * FileNotFoundException.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: magnanim
 */

#include "exceptions.h"

#include <sstream>

using namespace std;

DuplicateElementException::DuplicateElementException(string value) {
	DuplicateElementException::value = value;
}
DuplicateElementException::~DuplicateElementException() throw (){
	// TODO Auto-generated destructor stub
}
const char* DuplicateElementException::what() const throw()
{
    return value.data();
}
