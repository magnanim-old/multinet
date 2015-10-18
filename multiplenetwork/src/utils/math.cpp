/*
 * string.cpp
 *
 * Created on: Feb 8, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "utils.h"
#include <sstream>
#include <numeric>
#include <stdexcept>


namespace mlnet {

double mean(const std::vector<double>& vec) {
	// mean
	double sum = std::accumulate(vec.begin(), vec.end(), 0.0);
	return sum / vec.size();
}

double stdev(const std::vector<double>& vec) {
	// mean
	double sum = std::accumulate(vec.begin(), vec.end(), 0.0);
	double mean = sum / vec.size();
	// variance
	double variance = 0.0;
	for (double element: vec) {
		variance += (element - mean) * (element - mean);
	}
	variance /= vec.size();
	// standard deviation
	return std::sqrt(variance);
}

}
