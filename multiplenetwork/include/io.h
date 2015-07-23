#ifndef MULTIPLENETWORK_IO_H_
#define MULTIPLENETWORK_IO_H_

#include <exception>
#include <string>
#include "datastructures.h"


namespace mlnet {

/**
 * Reads a multiplex network from a list of edges.
 *
 * This is the accepted format:
 * ----------------------------------
 * -- comment lines start with two dashes (--)
 *
 * #TYPE [multiplex|multilayer]
 *
 * #LAYERS
 * LayerName,UNDIRECTED
 * LayerName,DIRECTED
 * -- etc.
 * -- If there are inter-layer edges, then their default directionality is indicated here:
 * LayerName1,LayerName2,DIRECTED
 * -- etc.
 *
 * #ACTOR ATTRIBUTES
 * AttributeName,STRING
 * AttributeName,NUMERIC
 * -- etc.
 *
 * #NODE ATTRIBUTES
 * LayerName,AttributeName,STRING
 * LayerName,AttributeName,NUMERIC
 * -- etc.
 *
 * #EDGE ATTRIBUTES
 * -- if type is multilayer, edge attributes are indicated as follows:
 * LayerName1,LayerName2,AttributeName,STRING
 * LayerName1,LayerName2,AttributeName,NUMERIC
 * -- etc.
 * -- if type is multiplex (default), edge attributes are indicated as follows:
 * LayerName,AttributeName,STRING
 * LayerName,AttributeName,NUMERIC
 * -- etc.
 *
 * #ACTORS
 * ActorName,AttributeList...
 * ActorName,AttributeList...
 * -- etc.
 *
 * #NODES
 * NodeName,LayerName,AttributeList...
 * NodeName,LayerName,AttributeList...
 * -- etc.
 *
 * #EDGES
 * -- if type is multilayer, edges are indicated as follows:
 * NodeName1,LayerName1,NodeName2,LayerName2,AttributeList...
 * -- etc.
 * -- if type is multiplex (default), edges are indicated as follows instead:
 * ActorName1,ActorName2,LayerName,AttributeList...
 * -- etc.
 * ----------------------------------
 *
 * If the #LAYERS section is empty, undirected layers are created as mentioned in the #EDGES section.
 *
 * Allowed values for #TYPE are "Multiplex" and "Multilayer", where multiplex is the default.
 *
 * If the #ACTOR ATTRIBUTES, #NODE ATTRIBUTES or #EDGE ATTRIBUTES sections are empty, no attributes are created.
 *
 * The #ACTORS and #NODES sections are useful only if attributes or disconnected nodes are present, otherwise they can be omitted
 *
 * If no section is specified, #EDGES is the default.
 *
 * Therefore, a minimalistic undirected network would look like this:
 * ----------------------------------
 * Matteo,Luca,Facebook
 * Matteo,Mark,Facebook
 * ...
 * ----------------------------------
 *
 * \param infile
 * \param network_name
 * \param separator
 *
 */
MLNetworkSharedPtr read_multilayer(const std::string& infile, const std::string& network_name, char separator);

void write_multilayer(const MLNetworkSharedPtr mlnet, const std::string& outfile, char separator);

void write_graphml(const MLNetworkSharedPtr mnet, const std::set<LayerSharedPtr>& layers, const std::string& path);

}

#endif /* MULTIPLENETWORK_IO_H_ */
