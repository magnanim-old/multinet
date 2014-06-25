/*
 * creation.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: magnanim
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "io.h"
#include "utils.h"
#include "datastructures.h"
#include "exceptions.h"

using namespace std;

MultiplexNetwork read_multiplex(const string& infile) {
	MultiplexNetwork mnet;
	enum Section {NETWORKS, EDGE_ATTRS, VERTEX_ATTRS, EDGES, VERTEXES};
	map<string,vector<bool> > network_edge_attr_type;
	map<string,vector<string> > network_edge_attr_names;
	map<string,vector<bool> > network_vertex_attr_type;
	map<string,vector<string> > network_vertex_attr_names;

	Section current_sect = EDGES; // Default section, in case a minimal network with just edges is used

	CSVReader csv;
	csv.open(infile);
	while (csv.hasNext()) {
		vector<string> v = csv.getNext();
		// SECTION
		if (v[0].find("#NETWORKS")!=string::npos) {
			current_sect = NETWORKS;
			continue;
		}
		if (v[0].find("#VERTEX ATTRIBUTES")!=string::npos) {
			current_sect = VERTEX_ATTRS;
			continue;
		}
		if (v[0].find("#EDGE ATTRIBUTES")!=string::npos) {
			current_sect = EDGE_ATTRS;
			continue;
		}
		if (v[0].find("#EDGES")!=string::npos) {
			current_sect = EDGES;
			continue;
		}
		if (v[0].find("#VERTEXES")!=string::npos) {
			current_sect = VERTEXES;
			continue;
		}
		if (current_sect == NETWORKS) {
			if (v.size()!=3) continue; //should we warn the user?
			string network_name = v[0];
			bool directed = (v[1]=="DIRECTED");
			bool weighted = (v[2]=="WEIGHTED");
			Network new_network(true,directed,weighted);
			mnet.addNetwork(network_name,new_network);
		}
		if (current_sect == VERTEX_ATTRS) {
			if (v.size()!=3) continue; //should we warn the user?
			string network_name = v[0];
			string attr_name = v[1];
			if (mnet.getNetwork(network_name).hasVertexAttribute(attr_name))
				continue;
			bool is_numeric = (v[2]=="NUMERIC");
			// Add control on STRING
			network_vertex_attr_type[network_name].push_back(is_numeric);
			network_vertex_attr_names[network_name].push_back(attr_name);
			if (is_numeric) mnet.getNetwork(network_name).newNumericVertexAttribute(attr_name);
			else mnet.getNetwork(network_name).newStringVertexAttribute(attr_name);
		}
		if (current_sect == EDGE_ATTRS) {
			if (v.size()!=3) continue; //should we warn the user?
			string network_name = v[0];
			string attr_name = v[1];
			if (mnet.getNetwork(network_name).hasEdgeAttribute(attr_name))
				continue; //should we warn the user?
			bool is_numeric = (v[2]=="NUMERIC");
			// Add control on STRING
			network_edge_attr_type[network_name].push_back(is_numeric);
			network_edge_attr_names[network_name].push_back(attr_name);
			if (is_numeric) mnet.getNetwork(network_name).newNumericEdgeAttribute(attr_name);
			else mnet.getNetwork(network_name).newStringEdgeAttribute(attr_name);}
		if (current_sect == VERTEXES) {

			string vertex_name = v[0];
			string network_name = v[1];

			if (!mnet.containsGlobalName(vertex_name)) {
				mnet.addGlobalName(vertex_name);
			}
			if (!mnet.containsNetwork(network_name)) {
				Network new_network(true,false,false);
				mnet.addNetwork(network_name,new_network);
				//cout << "added net\n";
			}
			if (!mnet.containsVertex(vertex_name,network_name)) {
				mnet.getNetwork(network_name).addVertex(vertex_name);
				mnet.mapIdentity(vertex_name,vertex_name,network_name);
				//cout << "added local from\n";
			}
			// Read vertex attributes
			if (2+network_vertex_attr_names[network_name].size()>v.size())
				throw ElementNotFoundException("not enough attribute values for vertex " + vertex_name);

			// indexes of current attribute in the local vectors and in the csv file.
			int attr_i=0;
			int csv_i=2;
			for (string attr_name: network_vertex_attr_names[network_name]) {
				if (network_vertex_attr_type[network_name][attr_i])
					mnet.getNetwork(network_name).setNumericVertexAttribute(vertex_name,attr_name,std::stod(v[csv_i],NULL));
				else mnet.getNetwork(network_name).setStringVertexAttribute(vertex_name,attr_name,v[csv_i]);
				attr_i++; csv_i++;
			}
		}
		if (current_sect == EDGES) {
			string from = v[0];
			string to = v[1];
			string network_name = v[2];

			//cout << from << " " << to << " " << network_name << "\n";
			if (!mnet.containsGlobalName(from)) {
				mnet.addGlobalName(from);
			}
			if (!mnet.containsGlobalName(to)) {
				mnet.addGlobalName(to);
				//cout << "added to\n";
			}
			if (!mnet.containsNetwork(network_name)) {
				Network new_network(true,false,false);
				mnet.addNetwork(network_name,new_network);
				//cout << "added net\n";
			}
			if (!mnet.containsVertex(from,network_name)) {
				mnet.getNetwork(network_name).addVertex(from);
				mnet.mapIdentity(from,from,network_name);
				//cout << "added local from\n";
			}
			if (!mnet.containsVertex(to,network_name)) {
				mnet.getNetwork(network_name).addVertex(to);
				mnet.mapIdentity(to,to,network_name);
				//cout << "added local to\n";
			}
			if (!mnet.getNetwork(network_name).containsEdge(from,to)) {
				if (mnet.getNetwork(network_name).isWeighted()) {
					double weight = std::stod(v[3],NULL);
					mnet.getNetwork(network_name).addEdge(from,to,weight);
				}
				else mnet.getNetwork(network_name).addEdge(from,to);
				//cout << "added edge\n";
			}
			// Read vertex attributes
			// indexes of current attribute in the local vectors and in the csv file.
			int attr_i=0;
			int csv_i=3;
			if (mnet.getNetwork(network_name).isWeighted()) csv_i++;
			if (csv_i+network_edge_attr_names[network_name].size()>v.size())
				throw ElementNotFoundException("not enough attribute values for edge (" + from + "," + to + ") on network " + network_name);

			for (string attr_name: network_edge_attr_names[network_name]) {
				if (network_edge_attr_type[network_name][attr_i])
					mnet.getNetwork(network_name).setNumericEdgeAttribute(from,to,attr_name,std::stod(v[csv_i],NULL));
				else mnet.getNetwork(network_name).setStringEdgeAttribute(from,to,attr_name,v[csv_i]);
				attr_i++; csv_i++;
			}
		}
	}
	return mnet;
}
