/*
 * exceptions.h
 *
 *  Created on: Feb 5, 2014
 *  Author: Matteo Magnani <matteo.magnani@it.uu.se>
 */

#ifndef MULTIPLENETWORK_IO_H_
#define MULTIPLENETWORK_IO_H_

#include <exception>
#include <string>
#include "datastructures.h"

void mnet_read_edgelist(MultipleNetwork&, std::string edge_file);

/**
 * Reads a multiple network from a list of edges.
 *
 * An edge is defined by two symbolic vertex names separated
 * by whitespace. (The symbolic vertex names themselves cannot contain
 * whitespaces). They must be followed by the name of the network.
 * They might be followed by an optional number indicating
 * the weight of the edge; the number can be negative and can be in
 * scientific notation. If there is no weight specified to an edge it
 * is assumed to be one.
 *
 * \param infile Pointer to a stream, it should be readable.
 *
 * Time complexity:
 *
 * @version 1.0 (Jun 12, 2013)
 * @author: Matteo Magnani <matteo.magnani@it.uu.se>
 */
/*
void mnet_read_edgelist(MultipleNetwork* mnet,
		int num_networks,
		FILE *instream[],
        std::string graph_names[],
        igraph_add_weights_t weights[],
        igraph_bool_t directed[]);

void mnet_read_edgelist(MultipleNetwork* mnet,
		std::string infile,
        std::string graph_names[],
        igraph_add_weights_t weights[],
        igraph_bool_t directed[]);
*/

#endif /* MULTIPLENETWORK_IO_H_ */
