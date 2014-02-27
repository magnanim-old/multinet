/*
 * ElementNotFoundException.cpp
 *
 *  Created on: Jul 1, 2013
 *  Author: Matteo Magnani <matteo.magnani@it.uu.se>
 */

#include "exceptions.h"

#include <sstream>

using namespace std;

// utility function
template <typename T> string number_to_string ( T Number ) {
     ostringstream ss;
     ss << Number;
     return ss.str();
}

ElementNotFoundException::ElementNotFoundException(string value) {
	ElementNotFoundException::value = value;
}
ElementNotFoundException::~ElementNotFoundException() throw (){
	// TODO Auto-generated destructor stub
}
const char* ElementNotFoundException::what() const throw()
{
    return value.data();
}
