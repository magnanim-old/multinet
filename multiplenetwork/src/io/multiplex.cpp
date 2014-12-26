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
	enum Section {TYPE, NETWORKS, ID_ATTRS, EDGE_ATTRS, VERTEX_ATTRS, IDENTITIES, EDGES, VERTEXES, DEFAULT};
	map<string,vector<bool> > network_edge_attr_type;
	map<string,vector<string> > network_edge_attr_names;
	map<string,vector<bool> > network_vertex_attr_type;
	map<string,vector<string> > network_vertex_attr_names;
	vector<bool> identity_attr_type;
	vector<string> identity_attr_names;

	Section current_sect = DEFAULT; // Default section, in case a minimal network with just edges is used

	CSVReader csv;
	csv.trimFields(true);
	csv.open(infile);
	while (csv.hasNext()) {
		vector<string> v = csv.getNext();
		if (v.size()==1 && v[0]=="") {
			continue;
		}
		// SECTION
		if (v[0].find("#TYPE")!=string::npos) {
			current_sect = TYPE;
			if (v.size()!=1 || (v[0]!="#TYPE multiplex" && v[0]!="#TYPE Multiplex" && v[0]!="#TYPE MULTIPLEX"))
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": only \"#TYPE multiplex\" signature currently supported");
			continue;
		}
		if (v[0].find("#NETWORKS")!=string::npos) {
			current_sect = NETWORKS;
			continue;
		}
		if (v[0].find("#IDENTITY ATTRIBUTES")!=string::npos) {
			current_sect = ID_ATTRS;
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
		if (v[0].find("#VERTEXES")!=string::npos || v[0].find("#VERTICES")!=string::npos) {
			current_sect = VERTEXES;
			continue;
		}
		if (v[0].find("#IDENTITIES")!=string::npos) {
			current_sect = IDENTITIES;
			continue;
		}
		if (current_sect == NETWORKS) {
			if (v.size()!=3)
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": DIRECTED/UNDIRECTED and WEIGHTED/UNWEIGHTED must be specified for each network");
			string network_name = v[0];
			bool directed = (v[1]=="DIRECTED" || v[1]=="directed" || v[2]=="Directed");
			bool weighted = (v[2]=="WEIGHTED" || v[2]=="weighted" || v[2]=="Weighted");
			Network new_network(true,directed,weighted);
			mnet.addNetwork(network_name,new_network);
		}
		if (current_sect == ID_ATTRS) {
			if (v.size()!=2)
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": for identity attributes, attribute name and type expected");
			string attr_name = v[0];
			if (mnet.hasAttribute(attr_name))
				continue;
			bool is_numeric = (v[1]=="NUMERIC");
			if (!is_numeric && v[1]!="STRING")
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": only NUMERIC or STRING attributes supported");
			identity_attr_type.push_back(is_numeric);
			identity_attr_names.push_back(attr_name);
			if (is_numeric) mnet.newNumericAttribute(attr_name);
			else mnet.newStringAttribute(attr_name);
		}
		if (current_sect == VERTEX_ATTRS) {
			if (v.size()!=3)
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": for vertex attributes, network name, attribute name and type expected");
			string network_name = v[0];
			string attr_name = v[1];
			if (mnet.getNetwork(network_name).hasVertexAttribute(attr_name))
				continue;
			bool is_numeric = (v[2]=="NUMERIC");
			if (!is_numeric && v[2]!="STRING")
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": only NUMERIC or STRING attributes supported");
			network_vertex_attr_type[network_name].push_back(is_numeric);
			network_vertex_attr_names[network_name].push_back(attr_name);
			if (is_numeric) mnet.getNetwork(network_name).newNumericVertexAttribute(attr_name);
			else mnet.getNetwork(network_name).newStringVertexAttribute(attr_name);
		}
		if (current_sect == EDGE_ATTRS) {
			if (v.size()!=3)
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": for edge attributes, network name, attribute name and type expected");
			string network_name = v[0];
			string attr_name = v[1];
			if (mnet.getNetwork(network_name).hasEdgeAttribute(attr_name))
				continue; //should we warn the user?
			bool is_numeric = (v[2]=="NUMERIC");
			if (!is_numeric && v[2]!="STRING")
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": only NUMERIC or STRING attributes supported");
			network_edge_attr_type[network_name].push_back(is_numeric);
			network_edge_attr_names[network_name].push_back(attr_name);
			if (is_numeric) mnet.getNetwork(network_name).newNumericEdgeAttribute(attr_name);
			else mnet.getNetwork(network_name).newStringEdgeAttribute(attr_name);}
		if (current_sect == IDENTITIES) {
			if (v.size()<1)
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": Identity name must be specified");

			string identity_name = v[0];

			if (!mnet.containsGlobalName(identity_name)) {
				mnet.addGlobalName(identity_name);
			}
			// Read attributes
			if (1+identity_attr_names.size()>v.size())
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": not enough attribute values for identity " + identity_name);

			// indexes of current attribute in the local vectors and in the csv file.
			int attr_i=0;
			int csv_i=1;
			for (string attr_name: identity_attr_names) {
				if (identity_attr_type[attr_i])
					mnet.setNumericAttribute(identity_name,attr_name,to_double(v[csv_i]));
				else mnet.setStringAttribute(identity_name,attr_name,v[csv_i]);
				attr_i++; csv_i++;
			}
		}
		if (current_sect == VERTEXES) {
			if (v.size()<2)
					throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": Vertex name and network name must be specified");

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
				mnet.map(vertex_name,vertex_name,network_name);
				//cout << "added local from\n";
			}
			// Read vertex attributes
			if (2+network_vertex_attr_names[network_name].size()>v.size())
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": not enough attribute values for vertex " + vertex_name);

			// indexes of current attribute in the local vectors and in the csv file.
			int attr_i=0;
			int csv_i=2;
			for (string attr_name: network_vertex_attr_names[network_name]) {
				if (network_vertex_attr_type[network_name][attr_i])
					mnet.getNetwork(network_name).setNumericVertexAttribute(vertex_name,attr_name,to_double(v[csv_i]));
				else mnet.getNetwork(network_name).setStringVertexAttribute(vertex_name,attr_name,v[csv_i]);
				attr_i++; csv_i++;
			}
		}
		if (current_sect == EDGES || current_sect == DEFAULT) {
			if (v.size()<3) {
				if (current_sect == EDGES) throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": From and To vertex names and network name must be specified for each edge");
				else if (current_sect == DEFAULT) throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": unrecognized statement");
			}
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
				mnet.map(from,from,network_name);
				//cout << "added local from\n";
			}
			if (!mnet.containsVertex(to,network_name)) {
				mnet.getNetwork(network_name).addVertex(to);
				mnet.map(to,to,network_name);
				//cout << "added local to\n";
			}
			if (!mnet.getNetwork(network_name).containsEdge(from,to)) {
				if (mnet.getNetwork(network_name).isWeighted()) {
					double weight = to_double(v[3]);
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
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": not enough attribute values for edge (" + from + "," + to + ") on network " + network_name);

			for (string attr_name: network_edge_attr_names[network_name]) {
				if (network_edge_attr_type[network_name][attr_i])
					mnet.getNetwork(network_name).setNumericEdgeAttribute(from,to,attr_name,to_double(v[csv_i]));
				else mnet.getNetwork(network_name).setStringEdgeAttribute(from,to,attr_name,v[csv_i]);
				attr_i++; csv_i++;
			}
		}
	}
	return mnet;
}

void write_multiplex(const MultiplexNetwork& mnet, const std::set<network_id>& networks, const string& path) {
	std::ofstream outfile;
	outfile.open(path.data());
	if (outfile.fail()) throw FileNotFoundException(path);

	outfile << "#TYPE multiplex" << std::endl;
	outfile << std::endl;

	outfile << "#NETWORKS" << std::endl;
	for (network_id network: networks) {
		outfile << mnet.getNetworkName(network);
		outfile << (mnet.getNetwork(network).isDirected()?",DIRECTED":",UNDIRECTED");
		outfile << (mnet.getNetwork(network).isWeighted()?",WEIGHTED":",UNWEIGHTED");
		outfile << std::endl;
	}
	outfile << std::endl;

	outfile << "#IDENTITY ATTRIBUTES" << std::endl;
	std::set<std::string> n_attrs = mnet.getNumericAttributes();
	for (std::string att: n_attrs)
		outfile << att << ",NUMERIC" << std::endl;
	std::set<std::string> s_attrs = mnet.getStringAttributes();
	for (std::string att: s_attrs)
		outfile << att << ",STRING" << std::endl;
	outfile << std::endl;

	outfile << "#VERTEX ATTRIBUTES" << std::endl;
	for (network_id network: networks) {
		std::string network_name = mnet.getNetworkName(network);
		std::set<std::string> n_attrs = mnet.getNetwork(network).getNumericVertexAttributes();
		for (std::string att: n_attrs)
			outfile << network_name << "," << att << ",NUMERIC" << std::endl;
		std::set<std::string> s_attrs = mnet.getNetwork(network).getStringVertexAttributes();
		for (std::string att: s_attrs)
			outfile << network_name << "," << att << ",STRING" << std::endl;
	}
	outfile << std::endl;

	outfile << "#EDGE ATTRIBUTES" << std::endl;
	for (network_id network: networks) {
		std::string network_name = mnet.getNetworkName(network);
		std::set<std::string> n_attrs = mnet.getNetwork(network).getNumericEdgeAttributes();
		for (std::string att: n_attrs)
			outfile << network_name << "," << att << ",NUMERIC" << std::endl;
		std::set<std::string> s_attrs = mnet.getNetwork(network).getStringEdgeAttributes();
		for (std::string att: s_attrs)
			outfile << network_name << "," << att << ",STRING" << std::endl;
	}
	outfile << std::endl;

	outfile << "#IDENTITIES" << std::endl;
	std::set<entity_id> identities = mnet.getGlobalIdentities();
	for (entity_id id: identities) {
		outfile << mnet.getGlobalName(id);
		std::set<std::string> n_attrs = mnet.getNumericAttributes();
		for (std::string att: n_attrs)
			outfile << "," << mnet.getNumericAttribute(id,att);
		std::set<std::string> s_attrs = mnet.getStringAttributes();
		for (std::string att: s_attrs)
			outfile << "," << mnet.getStringAttribute(id,att);
		outfile << std::endl;
	}
	outfile << std::endl;

	outfile << "#VERTEXES" << std::endl;
	for (network_id network: networks) {
		std::set<vertex_id> vertexes = mnet.getNetwork(network).getVertexes();
		for (vertex_id vid: vertexes) {
			outfile << mnet.getGlobalName(mnet.getGlobalIdentity(vid,network));
			outfile << "," << mnet.getNetworkName(network);
			std::set<std::string> n_attrs = mnet.getNetwork(network).getNumericVertexAttributes();
			for (std::string att: n_attrs)
				outfile << "," << mnet.getNetwork(network).getNumericVertexAttribute(vid,att);
			std::set<std::string> s_attrs = mnet.getNetwork(network).getStringVertexAttributes();
			for (std::string att: s_attrs)
				outfile << "," << mnet.getNetwork(network).getStringVertexAttribute(vid,att);
			outfile << std::endl;
		}
	}
	outfile << std::endl;

	outfile << "#EDGES" << std::endl;
	for (network_id network: networks) {
		std::set<edge_id> edges = mnet.getNetwork(network).getEdges();
		for (edge_id eid: edges) {
			outfile << mnet.getGlobalName(mnet.getGlobalIdentity(eid.v1,network));
			outfile << "," << mnet.getGlobalName(mnet.getGlobalIdentity(eid.v2,network));
			outfile << "," << mnet.getNetworkName(network);
			std::set<std::string> n_attrs = mnet.getNetwork(network).getNumericEdgeAttributes();
			for (std::string att: n_attrs)
				outfile << "," << mnet.getNetwork(network).getNumericEdgeAttribute(eid.v1,eid.v2,att);
			std::set<std::string> s_attrs = mnet.getNetwork(network).getStringEdgeAttributes();
			for (std::string att: s_attrs)
				outfile << "," << mnet.getNetwork(network).getStringEdgeAttribute(eid.v1,eid.v2,att);
			outfile << std::endl;
		}
	}
	outfile << std::endl;

	outfile.close();
}

void write_multiplex(const MultiplexNetwork& mnet, const std::vector<std::string>& networks, const string& path) {
	write_multiplex(mnet, network_names_to_ids(mnet,networks), path);
}


