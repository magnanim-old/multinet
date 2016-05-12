/*
 * utils.h
 *
 * Includes a number of utility classes/functions - see list below.
 */

#ifndef MLNET_UTILS_H_
#define MLNET_UTILS_H_

#include "utils/counter.h" // classes to count objects/values.
#include "utils/csv.h" // csv file reader.
#include "utils/propertymatrix.h" // representation of a network based on structures and observational functions.
#include "utils/math.h" // Basic mathematical functions and conversion from/to numbers/strings.
#include "utils/random.h" // functions based on random number generation.
#include "utils/sortedrandommap.h" // a class used to store a set of objects that can be accessed by id and by index.

namespace mlnet {

/*
typedef int verbosity;

const int VERBOSITY=2;
const int V_OFF=0;
const int V_BASIC=1;
const int V_VERBOSE=2;
const int V_DEBUG=3;

void log(const std::string& s);
void log(const std::string&, bool new_line);
void log(const std::string&, verbosity v);
void log(const std::string&, verbosity v, bool new_line);
void warn(const std::string& s);
void err(const std::string& s);

template <class T> void print_set(std::set<T>& input) {
	typename std::set<T>::iterator it;
	for (it=input.begin(); it!=input.end(); ++it) {
		if (it!=input.begin()) std::cout << ",";
		std::cout << (*it);
	}
	std::cout << std::endl;
}

*/

/***********************************/
/** Pretty printing               **/
/***********************************/
//void print(const MLNetworkSharedPtr& mnet);

}

#endif /* MLNET_UTILS_H_ */
