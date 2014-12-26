#ifndef MULTIPLENETWORK_IO_H_
#define MULTIPLENETWORK_IO_H_

#include <exception>
#include <string>
#include "datastructures.h"

/**
 * Reads a multiplex network from a list of edges.
 *
 * This is the accepted format:
 * ----------------------------------
 * #TYPE multiplex
 *
 * #NETWORKS
 * Facebook,UNDIRECTED,WEIGHTED
 * Twitter,DIRECTED,WEIGHTED
 *
 * #IDENTITY ATTRIBUTES
 * Surname,STRING
 * Age,NUMERIC
 *
 * #VERTEX ATTRIBUTES
 * Facebook,Color,STRING
 * Facebook,Profile_age,NUMERIC
 * Twitter,Profile_age,NUMERIC
 *
 * #EDGE ATTRIBUTES
 * Twitter,Stars,NUMERIC
 *
 * #IDENTITIES
 * Matteo,Magnani,34
 * Luca,Rossi,34
 * Rajesh,Sharma,33
 * Mostafa,Salehi,31
 * [more identities...]
 *
 * #VERTICES
 * Matteo,Facebook,Blue,34
 * Matteo,Twitter,33
 * [more vertices...]
 *
 * #EDGES
 * Matteo,Mostafa,Twitter,.3,6
 * Rajesh,Luca,Facebook,.5
 * [more edges...]
 * ----------------------------------
 *
 * If the #NETWORKS section is empty, undirected networks are created as mentioned in the #EDGES section.
 * Allowed values for #TYPE are "Multiplex" and "MultipleNetwork" (only the first, which is the default if not type is specified, has been implemented so far).
 * If the #IDENTITY ATTRIBUTES, #VERTEX ATTRIBUTES or #EDGE ATTRIBUTES sections are empty, no attributes are created.
 * The #IDENTITIES and #VERTICES sections are useful only if attributes or disconnected nodes are present.
 * If no section is specified, #EDGES is the default.
 *
 * Therefore, a minimalistic undirected network would look like this:
 * ----------------------------------
 * #EDGES
 * Matteo, Mostafa, Twitter
 * ...
 * ----------------------------------
 *
 * \param file
 *
 */
MultiplexNetwork read_multiplex(const std::string& infile);


void write_multiplex(const MultiplexNetwork& mnet, const std::set<network_id>& networks, const std::string& outfile);

void write_multiplex(const MultiplexNetwork& mnet, const std::vector<std::string>& networks, const std::string& outfile);


/**
 * Writes a network to file using the graphml format
 */
void write_graphml(const Network& net, const std::string& outfile);

void write_multi_graphml(const MultiplexNetwork& mnet, const std::set<network_id>& networks, const std::string& path);

void write_multi_graphml(const MultiplexNetwork& mnet, const std::vector<std::string>& networks, const std::string& path);

#endif /* MULTIPLENETWORK_IO_H_ */
