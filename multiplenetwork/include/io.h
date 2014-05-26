#ifndef MULTIPLENETWORK_IO_H_
#define MULTIPLENETWORK_IO_H_

#include <exception>
#include <string>
#include "datastructures.h"

/**
 * Reads a multiplex network from a list of edges. (the current version reads networks without attributes.)
 *
 * This is the accepted format:
 * ----------------------------------
 * #NETWORKS
 * Facebook,UNDIRECTED,WEIGHTED
 * Twitter,DIRECTED,WEIGHTED
 *
 * #VERTEX ATTRIBUTES
 * Facebook,Color,STRING
 * Facebook,Age,NUMERIC
 * Twitter,Age,NUMERIC
 *
 * #EDGE ATTRIBUTES
 * Twitter,Stars,NUMERIC
 *
 * #VERTICES
 * Matteo,Facebook,Blue,34
 * Matteo,Twitter,33
 * ...
 *
 * #EDGES
 * Matteo,Mostafa,Twitter,.3,6
 * ...
 * ----------------------------------
 *
 * If the #NETWORKS section is empty, undirected networks are created as mentioned in the #EDGES section.
 * Allowed values for #TYPE are "Multiplex" and "MultipleNetwork" (only the first implemented so far).
 * If the #VERTEX ATTRIBUTES or #EDGE ATTRIBUTES sections are empty, no attributes are created.
 * The #VERTICES section is useful only if attributes or disconnected nodes are present.
 *
 * Therefore, a minimalistic undirected network would look like this:
 * ----------------------------------
 * #EDGES
 * Matteo, Mostafa, Twitter, 32
 * ...
 * ----------------------------------
 *
 * \param file
 *
 */
void read_multiplex(MultiplexNetwork& mnet, const std::string& infile);


#endif /* MULTIPLENETWORK_IO_H_ */
