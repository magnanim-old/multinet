/*
 * FileNotFoundException.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: magnanim
 */

#include "exceptions.h"

#include <sstream>

using namespace std;

OperationNotSupportedException::OperationNotSupportedException(string value) {
	OperationNotSupportedException::value = "Operation not supported: " + value;
}
OperationNotSupportedException::~OperationNotSupportedException() throw (){
	// TODO Auto-generated destructor stub
}
const char* OperationNotSupportedException::what() const throw()
{
    return value.data();
}
