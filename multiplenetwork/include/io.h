#ifndef MULTIPLENETWORK_IO_H_
#define MULTIPLENETWORK_IO_H_

#include <exception>
#include <string>
#include <unordered_set>
#include "datastructures.h"

namespace mlnet {

/**
 * Reads a multilayer network from a list of edges.
 *
 * This is the accepted format:
 * ----------------------------------
 * -- comment lines start with two dashes (--)
 *
 * #TYPE multiplex
 * -- or: "#TYPE multilayer", if there are inter-layer edges
 *
 *
 * #LAYER ATTRIBUTES
 * AttributeName,STRING
 * AttributeName,NUMERIC
 * -- etc.
 *
 * #LAYERS
 * LayerName,UNDIRECTED,AttributeList...
 * LayerName,DIRECTED,AttributeList...
 * -- etc.
 * -- If there are inter-layer edges (that is,
 * -- TYPE is multilayer) then their default
 * -- directionality is indicated here:
 * -- LayerName1,LayerName2,DIRECTED
 * -- etc.
 *
 *
 * #ACTOR ATTRIBUTES
 * AttributeName,STRING
 * AttributeName,NUMERIC
 * -- etc.
 *
 * #ACTORS
 * ActorName,AttributeList...
 * ActorName,AttributeList...
 * -- etc.
 *
 *
 * #NODE ATTRIBUTES
 * LayerName,AttributeName,STRING
 * LayerName,AttributeName,NUMERIC
 * -- etc.
 *
 * #NODES
 * NodeName,LayerName,AttributeList...
 * NodeName,LayerName,AttributeList...
 * -- etc.
 *
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
 * #EDGES
 * -- if TYPE is multilayer, edges are indicated as follows:
 * ActorName1,LayerName1,ActorName2,LayerName2,AttributeList...
 * -- etc.
 * -- if TYPE is multiplex (default), edges are indicated as follows instead:
 * ActorName1,ActorName2,LayerName,AttributeList...
 * -- etc.
 * ----------------------------------
 *
 * If the #LAYERS section is empty, undirected layers are created as mentioned in the #EDGES section.
 *
 * If the #LAYER ATTRIBUTES, #ACTOR ATTRIBUTES, #NODE ATTRIBUTES or #EDGE ATTRIBUTES sections are empty, no attributes are created.
 *
 * The #LAYERS #ACTORS and #NODES sections are useful only if attributes or unused actors are present, otherwise they can be omitted
 *
 * If no section is specified, #EDGES is the default.
 *
 * Therefore, a minimalistic undirected multiplex network file would look like this:
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

void write_multilayer(const MLNetworkSharedPtr mlnet, const std::unordered_set<LayerSharedPtr>& layers, const std::string& outfile, char separator);

void write_graphml(const MLNetworkSharedPtr mnet, const std::unordered_set<LayerSharedPtr>& layers, const std::string& path, bool multigraph);

}

#endif /* MULTIPLENETWORK_IO_H_ */
