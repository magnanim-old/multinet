/**
 * random.h
 *
 * Functions based on random number generation
 */

#ifndef MLNET_RANDOM_H_
#define MLNET_RANDOM_H_

#include "exceptions.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>

namespace mlnet {

/***********************************/
/** Random                        **/
/***********************************/

/**
 * Returns a random integral number in the range [0,max[ using an
 * approximately uniform probability distribution.
 * @param max
 * @throws OperationNotSupprtedException if the range is larger than the number of random numbers that can be returned by the system
 */
int getRandomInt(int max);

/**
 * Returns a random integral number in the range [0,max[ using an
 * approximately uniform probability distribution.
 * @param max
 * @throws OperationNotSupprtedException if the range is larger than the number of random numbers that can be returned by the system
 */
long getRandomLong(long max);

/**
 * Returns a random double number in the range [0,1[ using an
 * approximately uniform probability distribution.
 */
double drand();

/**
 * Returns a number from 0 to MAX_LEVEL with geometrically decreasing
 * probability (returns 0 with probability (1-P), 1 with probability (1-P)*(1-P), etc.).
 */
int random_level(int MAX_LEVEL, double P);

/**
* Returns K random integral numbers in the range [0,max[ using an
* approximately uniform probability distribution.
* @param max
* @param k
*/
std::set<long> getKRandom(long max, long k);

/**
 * Random test: returns TRUE with probability probability.
 */
bool test(double probability);

/**
 * Random test: returns the index of the vector randomly selected,
 * where each element of the vector contains the probability of selecting
 * it. It is assumed that the elements of the vector sum up to 1.
 */
int test(const std::vector<double>& options);

/**
 * Random test: returns the index of the vector randomly selected,
 * where each element of the vector contains the probability of selecting
 * it. It is assumed that the elements of the vector sum up to 1.
 */
int test(const std::vector<std::vector<double> >& options, int row_num);

}

#endif /* MLNET_RANDOM_H_ */
