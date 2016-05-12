/*
 * print.h
 *
 */

#include "mlnetwork.h"

#ifndef MLNETWORK_PRINT_H_
#define MLNETWORK_PRINT_H_

void print(const mlnet::MLNetworkSharedPtr mnet);

template <typename T> void print(const mlnet::simple_set<T>& set) {
	std::cout << "{ ";
    for (T element: set)
    	std::cout << element << " ";
    std::cout << "}" << std::endl;
}

template <typename T> void print(const mlnet::sorted_set<T>& set) {
	std::cout << "{ ";
    for (T element: set)
    	std::cout << element << " ";
    std::cout << "}" << std::endl;
}

template <class STRUCTURE, class CONTEXT, class VALUE>
void print(const mlnet::property_matrix<STRUCTURE, CONTEXT, VALUE>& P) {
	for (auto s: P.columns()) {
    	std::cout << s << ":";
    	for (CONTEXT cont: P.rows()) {
    		std::cout << " (" << cont << ") " << P.get(s, cont);
    	}
        std::cout << std::endl;
	}
}

#endif /* MLNETWORK_PRINT_H_ */
