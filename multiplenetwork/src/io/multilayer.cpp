/*
 *  Created on: Jun 12, 2013
 *      Author: magnanim
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "ctype.h"
#include "io.h"
#include "utils.h"
#include "datastructures.h"
#include "exceptions.h"

using namespace std;

namespace mlnet {

MLNetworkSharedPtr read_multilayer(const string& infile, const string& network_name, char separator) {
	MLNetworkSharedPtr mnet = MLNetwork::create(network_name);
	enum NetType {MULTIPLEX, MULTILAYER};
	enum Section {TYPE, LAYERS, ACTOR_ATTRS, EDGE_ATTRS, NODE_ATTRS, ACTORS, EDGES, NODES, DEFAULT};
	enum EdgeDir {DIRECTED=true, UNDIRECTED=false};
	/********************************************/
	bool default_edge_directionality = UNDIRECTED;
	bool network_type = MULTIPLEX;
	/****************************/
	// C(++), I hate you...
	int (*touppercase)(int) = toupper;
	/****************************/
	map<string,map<string,vector<int> > > layers_edge_attr_type;
	map<string,map<string,vector<string> > > layers_edge_attr_names;
	map<string,vector<int> > layers_node_attr_type;
	map<string,vector<string> > layers_node_attr_names;
	map<string,map<string,bool> > edge_directionality;
	vector<int> actor_attr_type;
	vector<string> actor_attr_names;

	Section current_sect = DEFAULT; // Default section, in case only an edge list is provided

	CSVReader csv;
	csv.trimFields(true);
	csv.setFieldSeparator(separator);
	csv.open(infile);
	while (csv.hasNext()) {
		vector<string> v = csv.getNext();
		if (v.size()==1 && v[0]=="") {
			continue;
		}
		// SECTION
		if (v[0].find("#TYPE")!=string::npos) {
			current_sect = TYPE;
			std::transform(v[0].begin(), v[0].end(), v[0].begin(), touppercase);
			if (v[0]=="#TYPE MULTIPLEX")
				network_type=MULTIPLEX;
			else if (v[0]=="#TYPE MULTILAYER")
				throw OperationNotSupportedException("Multilayer format not implemented");
				//network_type=MULTILAYER; // TODO
			else throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": only \"multiplex\" and \"multilayer\" types are supported");
			continue;
		}
		if (v[0].find("#LAYERS")!=string::npos) {
			current_sect = LAYERS;
			continue;
		}
		if (v[0].find("#ACTOR ATTRIBUTES")!=string::npos) {
			current_sect = ACTOR_ATTRS;
			continue;
		}
		if (v[0].find("#NODE ATTRIBUTES")!=string::npos) {
			current_sect = NODE_ATTRS;
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
		if (v[0].find("#NODES")!=string::npos) {
			current_sect = NODES;
			continue;
		}
		if (v[0].find("#ACTORS")!=string::npos) {
			current_sect = ACTORS;
			continue;
		}
		if (current_sect == LAYERS) {
			if (v.size()!=2)
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": DIRECTED/UNDIRECTED must be specified for each layer");
			string layer_name = v[0];
			std::transform(v[1].begin(), v[1].end(), v[1].begin(), touppercase);
			bool directed = (v[1]=="DIRECTED");
			if (!directed && v[1]!="UNDIRECTED")
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": either DIRECTED or UNDIRECTED must be specified for each layer");
			mnet->add_layer(layer_name,directed);
		}
		if (current_sect == ACTOR_ATTRS) {
			if (v.size()!=2)
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": for actor attributes, attribute name and type expected");
			string attr_name = v[0];
			std::transform(v[1].begin(), v[1].end(), v[1].begin(), touppercase);
			if (mnet->actor_features()->attribute(attr_name)!=NULL)
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": for actor attributes, attribute name and type expected");
			attribute_type attr_type;
			if (v[1]=="NUMERIC") attr_type = NUMERIC_TYPE;
			else if (v[1]=="STRING") attr_type = STRING_TYPE;
			else throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": unsupported attribute type - " + v[1]);
			actor_attr_type.push_back(attr_type);
			actor_attr_names.push_back(attr_name);
			mnet->actor_features()->add(attr_name,attr_type);
		}
		if (current_sect == NODE_ATTRS) {
			if (v.size()!=3)
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": for node attributes, layer name, attribute name and attribute type expected");
			string layer_name = v[0];
			string attr_name = v[1];
			if (mnet->node_features(mnet->get_layer(layer_name))->attribute(attr_name)!=NULL)
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": for actor attributes, attribute name and type expected");
			attribute_type attr_type;
			std::transform(v[2].begin(), v[2].end(), v[2].begin(), touppercase);
			if (v[2]=="NUMERIC") attr_type = NUMERIC_TYPE;
			else if (v[2]=="STRING") attr_type = STRING_TYPE;
			else throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": unsupported attribute type - " + v[1]);
			layers_node_attr_type[layer_name].push_back(attr_type);
			layers_node_attr_names[layer_name].push_back(attr_name);
			mnet->node_features(mnet->get_layer(layer_name))->add(attr_name,attr_type);
		}
		if (current_sect == EDGE_ATTRS) {
			if (network_type==MULTIPLEX) {
				if (v.size()!=3)
					throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": for edge attributes, layer name, attribute name and type expected");
				string layer_name = v[0];
				LayerSharedPtr layer = mnet->get_layer(layer_name);
				string attr_name = v[1];
				if (mnet->edge_features(layer,layer)->attribute(attr_name)!=NULL)
					throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": for actor attributes, attribute name and type expected");
				attribute_type attr_type;
				std::transform(v[2].begin(), v[2].end(), v[2].begin(), touppercase);
				if (v[2]=="NUMERIC") attr_type = NUMERIC_TYPE;
				else if (v[2]=="STRING") attr_type = STRING_TYPE;
				else throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": unsupported attribute type - " + v[1]);
				layers_edge_attr_type[layer_name][layer_name].push_back(attr_type);
				layers_edge_attr_names[layer_name][layer_name].push_back(attr_name);
				mnet->edge_features(layer,layer)->add(attr_name,attr_type);
			}
			if (network_type==MULTILAYER) {
				if (v.size()!=4)
					throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": for edge attributes, layer names, attribute name and type expected");
				string layer_name1 = v[0];
				LayerSharedPtr layer1 = mnet->get_layer(layer_name1);
				string layer_name2 = v[1];
				LayerSharedPtr layer2 = mnet->get_layer(layer_name2);
				string attr_name = v[2];
				if (mnet->edge_features(layer1,layer2)->attribute(attr_name)!=NULL)
					throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": for actor attributes, attribute name and type expected");
				attribute_type attr_type;
				std::transform(v[3].begin(), v[3].end(), v[3].begin(), touppercase);
				if (v[3]=="NUMERIC") attr_type = NUMERIC_TYPE;
				else if (v[3]=="STRING") attr_type = STRING_TYPE;
				else throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": unsupported attribute type - " + v[2]);
				layers_edge_attr_type[layer_name1][layer_name2].push_back(attr_type);
				layers_edge_attr_names[layer_name1][layer_name2].push_back(attr_name);
				mnet->edge_features(layer1,layer2)->add(attr_name,attr_type);
			}
		}
		if (current_sect == ACTORS) {
			if (v.size()<1)
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": Actor name must be specified");
			string actor_name = v[0];
			ActorSharedPtr actor = mnet->get_actor(actor_name);
			if (actor==NULL) {
				actor = mnet->add_actor(actor_name);
			}
			// Read attributes
			if (1+actor_attr_names.size()>v.size())
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": not enough attribute values for actor " + actor_name);

			// indexes of current attribute in the local vectors and in the csv file.
			int attr_i=0;
			int csv_i=1;
			for (string attr_name: actor_attr_names) {
				switch (actor_attr_type[attr_i]) {
					case NUMERIC_TYPE:
						mnet->actor_features()->setNumeric(actor->id,attr_name,to_double(v[csv_i]));
						break;
					case STRING_TYPE:
						mnet->actor_features()->setString(actor->id,attr_name,v[csv_i]);
					break;
				}
				attr_i++; csv_i++;
			}
		}
		if (current_sect == NODES) {
			if (v.size()<2)
					throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": Node name and layer name must be specified");

			string node_name = v[0];
			string layer_name = v[1];

			// TODO Only for multiplex
			ActorSharedPtr actor = mnet->get_actor(node_name);
			if (actor==NULL) {
				actor = mnet->add_actor(node_name);
			}
			LayerSharedPtr layer = mnet->get_layer(layer_name);
			if (layer==NULL) {
				layer = mnet->add_layer(layer_name,default_edge_directionality);
			}
			NodeSharedPtr node = mnet->get_node(node_name,layer);
			if (node==NULL) {
				node = mnet->add_node(node_name,actor,layer);
			}

			// Read node attributes
			if (2+layers_node_attr_names[layer_name].size()>v.size())
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": not enough attribute values for node " + node_name);

			// indexes of current attribute in the local vectors and in the csv file.
			int attr_i=0;
			int csv_i=2;
			for (string attr_name: layers_node_attr_names[layer_name]) {
			switch (layers_node_attr_type[layer_name][attr_i]) {
				case NUMERIC_TYPE:
					mnet->node_features(layer)->setNumeric(node->id,attr_name,to_double(v[csv_i]));
					break;
				case STRING_TYPE:
					mnet->node_features(layer)->setString(node->id,attr_name,v[csv_i]);
				break;
				}
				attr_i++; csv_i++;
			}
		}
		if (current_sect == EDGES || current_sect == DEFAULT) {
			if (v.size()<3) {
				if (current_sect == EDGES) throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": From and To node names and layers name must be specified for each edge");
				else if (current_sect == DEFAULT) throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": unrecognized statement");
			}

			string from_node;
			string to_node;
			string layer_name1;
			string layer_name2;

			if (network_type==MULTIPLEX) {
				from_node = v[0];
				to_node = v[1];
				layer_name2 = layer_name1 = v[2];
			}
			else if (network_type==MULTILAYER) {
				from_node = v[0];
				layer_name1 = v[1];
				to_node = v[2];
				layer_name2 = v[3];
			}

			// TODO add warnings - and write MULTILAYER
			ActorSharedPtr actor1 = mnet->get_actor(from_node);
			if (actor1==NULL) {
				actor1 = mnet->add_actor(from_node);
			}
			ActorSharedPtr actor2 = mnet->get_actor(to_node);
			if (actor2==NULL) {
				actor2 = mnet->add_actor(to_node);
			}
			LayerSharedPtr layer1 = mnet->get_layer(layer_name1);
			if (layer1==NULL) {
				layer1 = mnet->add_layer(layer_name1,default_edge_directionality);
			}
			LayerSharedPtr layer2 = mnet->get_layer(layer_name2);
			if (layer2==NULL) {
				layer2 = mnet->add_layer(layer_name2,default_edge_directionality);
			}
			NodeSharedPtr node1 = mnet->get_node(from_node,layer1);
			if (node1==NULL) {
				node1 = mnet->add_node(from_node,actor1,layer1);
			}
			NodeSharedPtr node2 = mnet->get_node(to_node,layer2);
			if (node2==NULL) {
				node2 = mnet->add_node(to_node,actor2,layer2);
			}
			EdgeSharedPtr edge = mnet->get_edge(node1,node2);
			if (edge==NULL) {
				edge = mnet->add_edge(node1,node2);
			}
			// Read edge attributes
			// indexes of current attribute in the local vectors and in the csv file.
			int attr_i=0;
			int csv_i=0;
			if (network_type==MULTIPLEX) csv_i=3;
			else if (network_type==MULTILAYER) csv_i=4;
			if (csv_i+layers_edge_attr_names[layer_name1][layer_name2].size()>v.size())
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": not enough attribute values for edge " + edge->to_string());

			for (string attr_name: layers_edge_attr_names[layer_name1][layer_name2]) {
				switch (layers_edge_attr_type[layer_name1][layer_name2][attr_i]) {
				case NUMERIC_TYPE:
					mnet->edge_features(layer1,layer2)->setNumeric(edge->id,attr_name,to_double(v[csv_i]));
					break;
				case STRING_TYPE:
					mnet->edge_features(layer1,layer2)->setString(edge->id,attr_name,v[csv_i]);
				break;
				}
				attr_i++; csv_i++;
			}
		}
	}
	return mnet;
}

void write_multilayer(const MLNetworkSharedPtr mnet, const string& path, char sep) {
	std::ofstream outfile;
	outfile.open(path.data());
	if (outfile.fail()) throw FileNotFoundException(path);

	outfile << "#TYPE multiplex" << std::endl;
	outfile << std::endl;

	outfile << "#LAYERS" << std::endl;
	for (LayerSharedPtr layer: mnet->get_layers()) {
		outfile << layer->name << sep << "DIRECTED" << std::endl; // FIXME
	}

	outfile << "#ACTOR ATTRIBUTES" << std::endl;
	for (AttributeSharedPtr attr: mnet->actor_features()->attributes()) {
		outfile << attr->name()<< sep << attr->type_as_string() << std::endl;
	}

	outfile << "#NODE ATTRIBUTES" << std::endl;
	for (LayerSharedPtr layer: mnet->get_layers()) {
		for (AttributeSharedPtr attr: mnet->node_features(layer)->attributes()) {
			outfile << layer->name << sep << attr->name()<< sep << attr->type_as_string() << std::endl;
		}
	}

	outfile << "#EDGE ATTRIBUTES" << std::endl;
	for (LayerSharedPtr layer1: mnet->get_layers()) {
		for (LayerSharedPtr layer2: mnet->get_layers()) {
			for (AttributeSharedPtr attr: mnet->edge_features(layer1,layer2)->attributes()) {
				outfile << layer1->name << sep << layer2->name << sep << attr->name()<< sep << attr->type_as_string() << std::endl;
			}
		}
	}

	outfile << "#ACTORS" << std::endl;
	for (ActorSharedPtr actor: mnet->get_actors()) {
		outfile << actor->name;
		AttributeStoreSharedPtr actor_attrs = mnet->actor_features();
		for (AttributeSharedPtr attr: actor_attrs->attributes()) {
			switch (attr->type()) {
			case NUMERIC_TYPE:
				outfile << sep << actor_attrs->getNumeric(actor->id,attr->name());
				break;
			case STRING_TYPE:
				outfile << sep << actor_attrs->getString(actor->id,attr->name());
				break;
			}
		}
		outfile << std::endl;
	}

	outfile << "#NODES" << std::endl;
	for (LayerSharedPtr layer: mnet->get_layers()) {
		for (NodeSharedPtr node: mnet->get_nodes(layer)) {
			outfile << node->name << sep << node->layer->name;
			AttributeStoreSharedPtr node_attrs = mnet->node_features(layer);
			for (AttributeSharedPtr attr: node_attrs->attributes()) {
				switch (attr->type()) {
				case NUMERIC_TYPE:
					outfile << sep << node_attrs->getNumeric(node->id,attr->name());
					break;
				case STRING_TYPE:
					outfile << sep << node_attrs->getString(node->id,attr->name());
					break;
				}
			}
			outfile << std::endl;
		}
	}

	outfile << "#EDGES" << std::endl;
	for (LayerSharedPtr layer1: mnet->get_layers()) {
		for (LayerSharedPtr layer2: mnet->get_layers()) {
			for (EdgeSharedPtr edge: mnet->get_edges(layer1,layer2)) {
				outfile << edge->v1->name << sep << edge->v2->name << sep << edge->v2->layer->name;
				AttributeStoreSharedPtr edge_attrs = mnet->edge_features(layer1,layer2);
				for (AttributeSharedPtr attr: edge_attrs->attributes()) {
					switch (attr->type()) {
					case NUMERIC_TYPE:
						outfile << sep << edge_attrs->getNumeric(edge->id,attr->name());
						break;
					case STRING_TYPE:
						outfile << sep << edge_attrs->getString(edge->id,attr->name());
						break;
					}
				}
				outfile << std::endl;
			}
		}
	}
	outfile.close();
}

}
