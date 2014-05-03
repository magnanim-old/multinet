/*
 * creation.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: magnanim
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "io.h"
#include "utils.h"
#include "datastructures.h"

using namespace std;

/*void mnet_read_edgelist(MultipleNetwork* mnet,
		int num_networks,
		FILE *instream[],
        std::string graph_names[],
        igraph_add_weights_t weights[],
        igraph_bool_t directed[]) {

	igraph_i_set_attribute_table(&igraph_cattribute_table);
	for (int i=0; i<num_networks; i++) {
		//mnet.addNetwork(graph_names[i]);
		mnet.addNetwork();
		igraph_read_graph_ncol(mnet.getNetwork(i),instream[i],NULL,1,weights[i],directed[i]);
	}
	// set universe
	for (int i=0; i<num_networks; i++) {
		int num_vertexes = igraph_vcount(mnet.getNetwork(i));
		for (int node=0; node<num_vertexes; node++) {
			string node_name(igraph_cattribute_VAS(mnet.getNetwork(i), "name", node));
			mnet.addVertex(node_name);
		}
	}
}*/

/*void mnet_read_edgelist(MultipleNetwork& mnet,
		string infile,
        std::string graph_names[],
        igraph_add_weights_t weights[],
        igraph_bool_t directed[]) {


	igraph_i_set_attribute_table(&igraph_cattribute_table);
	for (int i=0; i<graph_names->length(); i++) {
		//mnet.addNetwork(graph_names[i]);
		mnet.addNetwork(graph_names[i],directed[i]);
	}

	CSVReader csv;
	csv.open(infile);
	while (csv.hasNext()) {
		vector<string> v = csv.getNext();
		string from = v[0];
		string to = v[1];
		string network_name = v[2];
		//cout << from << " " << to << " " << network_name << "\n";
		if (!mnet.containsGlobalVertex(from)) {
			mnet.addGlobalVertex(from);
			//cout << "added from\n";
		}
		if (!mnet.containsGlobalVertex(to)) {
			mnet.addGlobalVertex(to);
			//cout << "added to\n";
		}
		if (!mnet.containsNetwork(network_name)) {
			mnet.addNetwork(network_name,false);
			//cout << "added net\n";
		}
		if (!mnet.containsGlobalVertex(from,network_name)) {
			mnet.addLocalVertex(from,network_name);
			//cout << "added local from\n";
		}
		if (!mnet.containsGlobalVertex(to,network_name)) {
			mnet.addLocalVertex(to,network_name);
			//cout << "added local to\n";
		}
		if (!mnet.containsGlobalEdge(from,to,network_name)) {
			mnet.addGlobalEdge(from,to,network_name);
			//cout << "added edge\n";
		}
	}
}
*/

void mnet_read_edgelist(MultilayerNetwork& mnet, string infile) {
	CSVReader csv;
	csv.open(infile);
	while (csv.hasNext()) {
		vector<string> v = csv.getNext();
		string from = v[0];
		string to = v[1];
		string network_name = v[2];
		//cout << from << " " << to << " " << network_name << "\n";
		if (!mnet.containsVertex(from)) {
			mnet.addVertex(from);
			//cout << "added from\n";
		}
		if (!mnet.containsVertex(to)) {
			mnet.addVertex(to);
			//cout << "added to\n";
		}
		if (!mnet.containsNetwork(network_name)) {
			Network new_network(true,false,false);
			mnet.addNetwork(network_name,new_network);
			//cout << "added net\n";
		}
		if (!mnet.containsVertex(from,network_name)) {
			mnet.getNetwork(network_name)->addVertex(from);
			mnet.map(from,from,network_name);
			//cout << "added local from\n";
		}
		if (!mnet.containsVertex(to,network_name)) {
			mnet.getNetwork(network_name)->addVertex(to);
			mnet.map(to,to,network_name);
			//cout << "added local to\n";
		}
		if (!mnet.getNetwork(network_name)->containsEdge(from,to)) {
			mnet.getNetwork(network_name)->addEdge(from,to);
			//cout << "added edge\n";
		}
	}
}
