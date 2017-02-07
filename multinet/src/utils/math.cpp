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

double to_double(const std::string &double_as_string) {
	std::istringstream converted(double_as_string);
	double result;
	// maybe use some manipulators
	converted >> result;
	if (!converted)
		throw std::runtime_error("Error converting to double");
	return result;
}

int to_int(const std::string &int_as_string) {
	std::istringstream converted(int_as_string);
	int result;
	// maybe use some manipulators
	converted >> result;
	if (!converted)
		throw std::runtime_error("Error converting to integer");
	return result;
}


}
