/*
 * string.cpp
 *
 * Created on: Feb 8, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "utils.h"
#include <sstream>
#include <stdexcept>

double to_double(const std::string &double_as_string) {
    std::istringstream converted(double_as_string);
    double double_as_double;
    // maybe use some manipulators
    converted >> double_as_double;
    if (!converted)
        throw std::runtime_error("Error converting to double");
    return double_as_double;
}
