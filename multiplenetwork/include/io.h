/**
 * io.h
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 1.0
 *
 * Methods to read/write a multilayer network from/to file.
 */

#ifndef MLNET_IO_H_
#define MLNET_IO_H_

#include "datastructures.h"

namespace mlnet {

/**
 * Reads a multilayer network from a list of edges. There are two main formats available. For multiplex
 * networks, were edges are only present inside a layer, edges are expressed as Actor1,Actor2,Layer.
 * For general multilayer networks, where edges can connect actors on different layers, edges are
 * expressed as Actor1,Layer1,Actor2,Layer2.
 * Directed/undirected edges, as well as attributes, are also supported. This is the complete accepted format:
 *
 * ----------------------------------
 * -- comment lines start with two dashes (--)
 *
 * #TYPE multiplex
 * -- or: "#TYPE multilayer", if there are inter-layer edges
 *
 * #ACTOR ATTRIBUTES
 * AttributeName1,STRING
 * AttributeName2,NUMERIC
 * -- etc.
 *
 * #NODE ATTRIBUTES
 * LayerName1,AttributeName1,STRING
 * LayerName1,AttributeName2,NUMERIC
 * -- etc.
 *
 * #EDGE ATTRIBUTES
 * -- if type is multiplex (default), edge attributes are indicated as follows:
 * LayerName1,AttributeName,STRING
 * LayerName2,AttributeName,NUMERIC
 * -- etc.
 * -- if type is multilayer, edge attributes are indicated as follows:
 * -- LayerName1,LayerName2,AttributeName,STRING
 * -- LayerName1,LayerName2,AttributeName,NUMERIC
 * -- etc.
 *
 *
 * #LAYERS
 * -- if type is multiplex (default), edge attributes are indicated as follows:
 * LayerName1,UNDIRECTED,AttributeValueList...
 * LayerName2,DIRECTED,AttributeValueList...
 * -- etc.
 * -- if type is multilayer, layers are indicated as follows:
 * -- LayerName1,LayerName1,UNDIRECTED,AttributeValueList...
 * -- LayerName2,LayerName2,DIRECTED,AttributeValueList...
 * -- LayerName1,LayerName2,DIRECTED
 * -- etc. and all intra-layer directionalities should be listed before inter-layer directionalities
 *
 * #ACTORS
 * ActorName1,AttributeValueList...
 * ActorName2,AttributeValueList...
 * -- etc.
 *
 * #NODES
 * ActorName1,LayerName1,AttributeValueList...
 * ActorName1,LayerName2,AttributeValueList...
 * -- etc.
 *
 * #EDGES
 * -- if TYPE is multiplex (default), edges are indicated as follows instead:
 * ActorName1,ActorName2,LayerName1,AttributeValueList...
 * -- etc.
 * -- if TYPE is multilayer, edges are indicated as follows:
 * -- ActorName1,LayerName1,ActorName2,LayerName2,AttributeValueList...
 * -- etc.
 *
 * ----------------------------------
 *
 * If the type is Multiplex and there is no #NODES section, all actors are automatically added to all layers
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
 * @param infile path of the file storing the multilayer network
 * @param network_name name assigned to the network
 * @param separator character used to separate fields in the input file
 *
 */
MLNetworkSharedPtr read_multilayer(const std::string& infile, const std::string& network_name, char separator);

/**
* Reads a simple FULLMATRIX DIAGONAL PRESENT dl file with multiple matrices. The dl format is more complex than this:
* this function is not intended to read all possible dl files, but can be used as a starting point...
* @param infile path of the file storing the multilayer network
* @param network_name name assigned to the network
*
*/
MLNetworkSharedPtr read_dl(const std::string& infile, const std::string& network_name, const vector<bool>& symmetric, const vector<bool>& valued);

/**
 * This method writes a multilayer network to file, using the same format described for the read_multilayer method
 * @param mlnet network to be saved to file
 * @param outfile path of the file where to store the multilayer network
 * @param separator character to be used to separate fields in the output file
 *
 */
void write_multilayer(const MLNetworkSharedPtr& mlnet, const std::string& outfile, char separator);

/**
 * This method writes a multiplex network to file, using the same format described for the read_multilayer method.
 * Interlayer edges are discarded if present - no check is performed.
 * @param mlnet network to be saved to file
 * @param outfile path of the file where to store the multilayer network
 * @param separator character to be used to separate fields in the output file
 *
 */
void write_multiplex(const MLNetworkSharedPtr& mlnet, const std::string& outfile, char separator);


/**
 * This method writes a multiplex network to file, using the same format described for the read_multilayer method.
 * Interlayer edges are discarded if present - no check is performed.
 * Actor, layer and node attributes are also discarded.
 * @param mlnet network to be saved to file
 * @param outfile path of the file where to store the multilayer network
 * @param separator character to be used to separate fields in the output file
 *
 */
void write_multiplex_edgelist(const MLNetworkSharedPtr& mlnet, const std::string& outfile, char separator);

/**
 * This method writes a multiplex network to file, using the same format described for the read_multilayer method.
 * Interlayer edges are discarded if present - no check is performed.
 * Actor, layer and node attributes are also discarded.
 * In addition, actor names are replaced with random identifiers.
 * @param mlnet network to be saved to file
 * @param outfile path of the file where to store the multilayer network
 * @param separator character to be used to separate fields in the output file
 *
 */
void write_anonymized_multiplex_edgelist(const MLNetworkSharedPtr& mnet, const string& path, char sep);

/**
 * This method writes a multilayer network to an XML file, using the graphML format. This is widely
 * used, and allows the network to be opened with other network analysis software. The method allows
 * to save the most typical graph-based representations of the multilayer network: multiplex, multigraph,
 * or keeping the nodes in different layers separated, representing the original information as node and
 * edge attributes. If other variations are needed, the original multilayer network can be transformed
 * before exporting it.
 * @param mlnet network to be saved to file
 * @param outfile path of the file where to store the multilayer network
 * @param layers only the layers in this set are included in the output
 * @param merge_actors if true, one node in the output graph will represent all nodes associated to the same
 * actor in the original network. If false, for each node in the original network a node will appear in the output,
 * with an attribute indicating the corresponding actor. The first option builds a "traditional" multiplex network.
 * @param include_all_actors if parameter merge_actors is true, then we can decide which actors to include. If include_all_actors is true, all the actors in the multilayer network are included in the output. If false, only
 * actors that are present in the exported layers are included (see layers parameter). If merge_actors is false, this attribute is not used.
 */
void write_graphml(const MLNetworkSharedPtr& mnet, const string& outfile, const hash_set<LayerSharedPtr>& layers, bool merge_actors, bool include_all_actors);

/**
 * Utility function to read attribute values.
 * @param store attribute store where the attribute values are kept for the input object
 * @param id identifier of the object for which the attributes should be read
 * @param attr_types vector with the expected types of attributes
 * @param attr_names vector with the expected names of attributes
 * @param line a vector of strings where the attribute values are stores
 * @param idx the index of the first attribute value in the line vector
 * @param csv_line_number the line of the CSV file at which the line vector has been read, for error management
 */
void read_attributes(const AttributeStoreSharedPtr& store, object_id id, const vector<int>& attr_types, const vector<string>& attr_names, const vector<string>& line, int idx, int csv_line_number);

}

#endif /* MLNET_IO_H_ */
