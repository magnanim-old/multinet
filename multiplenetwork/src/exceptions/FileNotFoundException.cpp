/*
 * FileNotFoundException.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: magnanim
 */

#include "exceptions.h"

using namespace std;

FileNotFoundException::FileNotFoundException(string path) {
	FileNotFoundException::path = path;
}
FileNotFoundException::~FileNotFoundException() throw (){
	// TODO Auto-generated destructor stub
}
const char* FileNotFoundException::what() const throw()
{
    return (string("File not found: ") + path).data();
}
