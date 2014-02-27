/*
 * string.cpp
 *
 * Created on: Feb 8, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "utils.h"
#include <sstream>


std::string to_string (long number) {
     std::ostringstream ss;
     ss << number;
     return ss.str();
}
