#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <string>
#include "ctype.h"
#include "io.h"
#include "utils.h"
#include "datastructures.h"
#include "exceptions.h"

using namespace std;

namespace mlnet {

MLNetworkSharedPtr read_multilayer(const string& infile, const string& network_name, char separator) {
	//std::cout << "create" << std::endl;
	MLNetworkSharedPtr mnet = MLNetwork::create(network_name);
	enum NetType {MULTIPLEX_NETWORK, MULTILAYER_NETWORK};
	enum Section {TYPE, LAYERS, ACTOR_ATTRS, EDGE_ATTRS, NODE_ATTRS, ACTORS, EDGES, NODES, DEFAULT};
	/********************************************/
	NetType network_type = MULTIPLEX_NETWORK;
	// Wether the file contains or not a #NODES section. If not, and type is  MULTIPLEX_NETWORK, by default all actors are added to all layers
	bool true_multiplex = false;
	/****************************/
	// C(++), I hate you...
	//std::cout << "hate" << std::endl;
	int (*touppercase)(int) = toupper;
	//std::cout << "you" << std::endl;
	/****************************/
	vector<int> actor_attr_type;
	vector<string> actor_attr_names;
	vector<int> layer_attr_type;
	vector<string> layer_attr_names;
	hash_map<string,hash_map<string,vector<int> > > edge_attr_type;
	hash_map<string,hash_map<string,vector<string> > > edge_attr_names;
	hash_map<string,vector<int> > node_attr_type;
	hash_map<string,vector<string> > node_attr_names;
	hash_map<string,hash_map<string,edge_directionality> > _edge_directionality;

	// Default section, in case only an edge list is provided
	Section current_sect = DEFAULT;

	CSVReader csv;
	csv.trimFields(true);
	csv.setFieldSeparator(separator);
	//std::cout << "open " << infile << "..." << std::endl;
	csv.open(infile);
	//std::cout << "opened" << std::endl;
	while (csv.hasNext()) {
		vector<string> v = csv.getNext();
		//if (csv.rowNum() % 100000 == 0)
		//	std::cout << "row " << csv.rowNum() << std::endl;
		if (v.size()==1 && v.at(0)=="") {
			continue;
		}

		// TODO Does not work if separator is " "
		// SECTION IDENTIFICATION
		if (v[0].find("#TYPE")!=string::npos) {
			current_sect = TYPE;
			std::transform(v[0].begin(), v[0].end(), v[0].begin(), touppercase);
			if (v[0]=="#TYPE MULTIPLEX")
				network_type=MULTIPLEX_NETWORK;
			else if (v[0]=="#TYPE MULTILAYER")
				network_type=MULTILAYER_NETWORK;
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
		if (current_sect == TYPE && network_type == MULTIPLEX_NETWORK) {
			std::transform(v[0].begin(), v[0].end(), v[0].begin(), touppercase);
			if (v[0]=="ALL NODES")
				true_multiplex=true;
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
			node_attr_type[layer_name].push_back(attr_type);
			node_attr_names[layer_name].push_back(attr_name);
			mnet->node_features(mnet->get_layer(layer_name))->add(attr_name,attr_type);
		}
		if (current_sect == EDGE_ATTRS) {
			if (network_type==MULTIPLEX_NETWORK) {
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
				edge_attr_type[layer_name][layer_name].push_back(attr_type);
				edge_attr_names[layer_name][layer_name].push_back(attr_name);
				mnet->edge_features(layer,layer)->add(attr_name,attr_type);
			}
			else if (network_type==MULTILAYER_NETWORK) {
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
				edge_attr_type[layer_name1][layer_name2].push_back(attr_type);
				edge_attr_names[layer_name1][layer_name2].push_back(attr_name);
				mnet->edge_features(layer1,layer2)->add(attr_name,attr_type);
			}
		}
		if (current_sect == LAYERS) {
			string layer_name1, layer_name2;
			string directionality;
			if (network_type==MULTIPLEX_NETWORK) {
				if (v.size()<2) {
					throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": expected layer name followed by DIRECTED/UNDIRECTED");
				}
				layer_name1 = layer_name2 = v[0];
				directionality = v[1];
			}
			else if (network_type==MULTILAYER_NETWORK) {
				if (v.size()<3) {
					throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": expected two layer names followed by DIRECTED/UNDIRECTED");
				}
				layer_name1 = v[0];
				layer_name2 = v[1];
				directionality = v[2];
			}
			std::transform(directionality.begin(), directionality.end(), directionality.begin(), touppercase);
			edge_directionality dir;
			if (directionality=="DIRECTED") dir = DIRECTED;
			else if (directionality=="UNDIRECTED") dir = UNDIRECTED;
			else throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": either DIRECTED or UNDIRECTED must be specified");
			if (layer_name1==layer_name2) {
				LayerSharedPtr layer = mnet->add_layer(layer_name1,dir);
			}
			else {
				// TODO first single layers must be specified
				LayerSharedPtr layer1 = mnet->get_layer(layer_name1);
				LayerSharedPtr layer2 = mnet->get_layer(layer_name2);
				mnet->set_directed(layer1,layer2,((dir==DIRECTED)?true:false));
			}
		}
		if (current_sect == ACTORS) {
			if (v.size()<1)
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": Actor name must be specified");
			string actor_name = v[0];
			ActorSharedPtr actor = mnet->add_actor(actor_name);
			if (actor==NULL) { // actor already present
				actor = mnet->get_actor(actor_name);
			}
			read_attributes(mnet->actor_features(), actor->id, actor_attr_type, actor_attr_names, v, 1, csv.rowNum());
		}
		if (current_sect == NODES) {
			//std::cout << "nodes" << std::endl;

			if (v.size()<2)
					throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": Node name and layer name must be specified");

			string node_name = v[0];
			string layer_name = v[1];

			ActorSharedPtr actor = mnet->get_actor(node_name);
			if (actor==NULL) {
				actor = mnet->add_actor(node_name);
			}
			LayerSharedPtr layer = mnet->get_layer(layer_name);
			if (layer==NULL) {
				layer = mnet->add_layer(layer_name,DEFAULT_EDGE_DIRECTIONALITY);
			}

			NodeSharedPtr node = mnet->get_node(actor,layer);
			if (node==NULL) {
				node = mnet->add_node(actor,layer);
			}
			read_attributes(mnet->node_features(layer), node->id, node_attr_type[layer_name], node_attr_names[layer_name], v, 2, csv.rowNum());
		}
		if (current_sect == EDGES || current_sect == DEFAULT) {
			//std::cout << "edges" << std::endl;

			if (v.size()<3) {
				if (current_sect == EDGES) throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": From and To node names and layers name must be specified for each edge");
				else if (current_sect == DEFAULT) throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": unrecognized statement");
			}

			string from_node;
			string to_node;
			string layer_name1;
			string layer_name2;

			if (network_type==MULTIPLEX_NETWORK) {
				from_node = v[0];
				to_node = v[1];
				layer_name2 = layer_name1 = v[2];
			}
			else if (network_type==MULTILAYER_NETWORK) {
				from_node = v[0];
				layer_name1 = v[1];
				to_node = v[2];
				layer_name2 = v[3];
			}

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
				layer1 = mnet->add_layer(layer_name1,DEFAULT_EDGE_DIRECTIONALITY);
			}

			LayerSharedPtr layer2 = mnet->get_layer(layer_name2);
			if (layer2==NULL) {
				layer2 = mnet->add_layer(layer_name2,DEFAULT_EDGE_DIRECTIONALITY);
			}

			NodeSharedPtr node1 = mnet->get_node(actor1,layer1);
			if (node1==NULL) {
				node1 = mnet->add_node(actor1,layer1);
			}

			NodeSharedPtr node2 = mnet->get_node(actor2,layer2);
			if (node2==NULL) {
				node2 = mnet->add_node(actor2,layer2);
			}

			EdgeSharedPtr edge = mnet->get_edge(node1,node2);
			//std::cout << edge << std::endl;
			if (edge==NULL) {
				edge = mnet->add_edge(node1,node2);
			}

			if (network_type==MULTIPLEX_NETWORK)
				read_attributes(mnet->edge_features(layer1,layer2), edge->id, edge_attr_type[layer_name1][layer_name2], edge_attr_names[layer_name1][layer_name2], v, 3, csv.rowNum());
			else if (network_type==MULTILAYER_NETWORK)
				read_attributes(mnet->edge_features(layer1,layer2), edge->id, edge_attr_type[layer_name1][layer_name2], edge_attr_names[layer_name1][layer_name2], v, 4, csv.rowNum());
		}
	}
	if (true_multiplex) {
		// Init nodes
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			for (ActorSharedPtr actor: *mnet->get_actors()) {
				mnet->add_node(actor,layer);
			}
		}
	}
	return mnet;
}

    
    void write_multilayer(const MLNetworkSharedPtr& mnet, const string& path, char sep) {
        hash_set<LayerSharedPtr> layers;
        for (LayerSharedPtr layer: *mnet->get_layers()) {
            layers.insert(layer);
        }
        write_multilayer(mnet, path, layers, sep);
    }
    
void write_multilayer(const MLNetworkSharedPtr& mnet, const string& path, const hash_set<LayerSharedPtr>& layers, char sep) {
	std::ofstream outfile;
	outfile.open(path.data());

	outfile << "#TYPE multilayer" << std::endl;
	outfile << std::endl;

	outfile << "#LAYERS" << std::endl;
	for (LayerSharedPtr layer: *mnet->get_layers()) {
        if (layers.count(layer)==0) continue;
		outfile << layer->name << sep << layer->name << sep << (mnet->is_directed(layer,layer)?"DIRECTED":"UNDIRECTED") << std::endl;
	}
	for (LayerSharedPtr layer1: *mnet->get_layers()) {
        if (layers.count(layer1)==0) continue;
		for (LayerSharedPtr layer2: *mnet->get_layers()) {
            if (layers.count(layer2)==0) continue;
			if (layer1==layer2)
				continue;
			outfile << layer1->name << sep << layer2->name << sep << (mnet->is_directed(layer1,layer2)?"DIRECTED":"UNDIRECTED") << std::endl;
		}
	}
	outfile << std::endl;

	outfile << "#ACTOR ATTRIBUTES" << std::endl;
	for (AttributeSharedPtr attr: mnet->actor_features()->attributes()) {
		outfile << attr->name() << sep << attr->type_as_string() << std::endl;
	}
	outfile << std::endl;

	outfile << "#NODE ATTRIBUTES" << std::endl;
	for (LayerSharedPtr layer: *mnet->get_layers()) {
        if (layers.count(layer)==0) continue;
		for (AttributeSharedPtr attr: mnet->node_features(layer)->attributes()) {
			outfile << layer->name << sep << attr->name()<< sep << attr->type_as_string() << std::endl;
		}
	}
	outfile << std::endl;

	outfile << "#EDGE ATTRIBUTES" << std::endl;
	for (LayerSharedPtr layer1: *mnet->get_layers()) {
        if (layers.count(layer1)==0) continue;
		for (LayerSharedPtr layer2: *mnet->get_layers()) {
            if (layers.count(layer2)==0) continue;
			for (AttributeSharedPtr attr: mnet->edge_features(layer1,layer2)->attributes()) {
				outfile << layer1->name << sep << layer2->name << sep << attr->name()<< sep << attr->type_as_string() << std::endl;
			}
		}
	}
	outfile << std::endl;

	outfile << "#ACTORS" << std::endl;
	for (ActorSharedPtr actor: *mnet->get_actors()) {
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
	outfile << std::endl;

	outfile << "#NODES" << std::endl;
	for (LayerSharedPtr layer: *mnet->get_layers()) {
        if (layers.count(layer)==0) continue;
		for (NodeSharedPtr node: *mnet->get_nodes(layer)) {
			outfile << node->actor->name << sep << node->layer->name;
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
	outfile << std::endl;

	outfile << "#EDGES" << std::endl;
	for (LayerSharedPtr layer1: *mnet->get_layers()) {
        if (layers.count(layer1)==0) continue;
		for (LayerSharedPtr layer2: *mnet->get_layers()) {
            if (layers.count(layer2)==0) continue;
			for (EdgeSharedPtr edge: *mnet->get_edges(layer1,layer2)) {
				outfile << edge->v1->actor->name << sep << edge->v1->layer->name << sep << edge->v2->actor->name << sep << edge->v2->layer->name;
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

    
    void write_multiplex(const MLNetworkSharedPtr& mnet, const string& path, char sep) {
        hash_set<LayerSharedPtr> layers;
        for (LayerSharedPtr layer: *mnet->get_layers()) {
            layers.insert(layer);
        }
        write_multiplex(mnet, path, layers, sep);
    }

	void write_multiplex(const MLNetworkSharedPtr& mnet, const string& path, const hash_set<LayerSharedPtr>& layers, char sep) {
		std::ofstream outfile;
		outfile.open(path.data());

		outfile << "#TYPE multiplex" << std::endl;
		outfile << std::endl;

		outfile << "#LAYERS" << std::endl;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
            if (layers.count(layer)==0) continue;
			outfile << layer->name << sep << (mnet->is_directed(layer,layer)?"DIRECTED":"UNDIRECTED") << std::endl;
		}
		outfile << std::endl;

		outfile << "#ACTOR ATTRIBUTES" << std::endl;
		for (AttributeSharedPtr attr: mnet->actor_features()->attributes()) {
			outfile << attr->name() << sep << attr->type_as_string() << std::endl;
		}
		outfile << std::endl;

		outfile << "#NODE ATTRIBUTES" << std::endl;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
            if (layers.count(layer)==0) continue;
			for (AttributeSharedPtr attr: mnet->node_features(layer)->attributes()) {
				outfile << layer->name << sep << attr->name()<< sep << attr->type_as_string() << std::endl;
			}
		}
		outfile << std::endl;

		outfile << "#EDGE ATTRIBUTES" << std::endl;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
            if (layers.count(layer)==0) continue;
			for (AttributeSharedPtr attr: mnet->edge_features(layer,layer)->attributes()) {
				outfile << layer->name << sep << attr->name()<< sep << attr->type_as_string() << std::endl;
			}
		}
		outfile << std::endl;

		outfile << "#ACTORS" << std::endl;
		for (ActorSharedPtr actor: *mnet->get_actors()) {
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
		outfile << std::endl;

		outfile << "#NODES" << std::endl;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
            if (layers.count(layer)==0) continue;
			for (NodeSharedPtr node: *mnet->get_nodes(layer)) {
				outfile << node->actor->name << sep << node->layer->name;
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
		outfile << std::endl;

		outfile << "#EDGES" << std::endl;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
            if (layers.count(layer)==0) continue;
			for (EdgeSharedPtr edge: *mnet->get_edges(layer,layer)) {
					outfile << edge->v1->actor->name << sep << edge->v2->actor->name << sep << edge->v2->layer->name;
					AttributeStoreSharedPtr edge_attrs = mnet->edge_features(layer,layer);
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
		outfile.close();
	}

	void write_multiplex_edgelist(const MLNetworkSharedPtr& mnet, const string& path, char sep) {
		std::ofstream outfile;
		outfile.open(path.data());


		bool has_directed_layers = false;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			if (mnet->is_directed(layer,layer)) {
				has_directed_layers = true;
				break;
			}
		}
		if (has_directed_layers) {
		outfile << "#LAYERS" << std::endl;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			outfile << layer->name << sep << (mnet->is_directed(layer,layer)?"DIRECTED":"UNDIRECTED") << std::endl;
		}
		outfile << std::endl;
		}

		bool has_edge_attributes = false;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			if (mnet->edge_features(layer,layer)->attributes().size()>0){
				has_edge_attributes = true;
				break;
			}
		}
		if (has_edge_attributes) {
		outfile << "#EDGE ATTRIBUTES" << std::endl;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			for (AttributeSharedPtr attr: mnet->edge_features(layer,layer)->attributes()) {
				outfile << layer->name << sep << attr->name()<< sep << attr->type_as_string() << std::endl;
			}
		}
		outfile << std::endl;
		}

		if (has_directed_layers || has_edge_attributes)
			outfile << "#EDGES" << std::endl;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			for (EdgeSharedPtr edge: *mnet->get_edges(layer,layer)) {
					outfile << edge->v1->actor->name << sep << edge->v2->actor->name << sep << edge->v2->layer->name;
					AttributeStoreSharedPtr edge_attrs = mnet->edge_features(layer,layer);
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
		outfile.close();
	}

	void write_anonymized_multiplex_edgelist(const MLNetworkSharedPtr& mnet, const string& path, char sep) {
		std::ofstream outfile;
		outfile.open(path.data());

		// generate random ids
		vector<string> names(mnet->get_actors()->size());
		for (uint i = 0; i<names.size(); i++) {
			names[i] = "A" + to_string(i);
		}
		std::random_shuffle(names.begin(),names.end());
		// map actors to the ids
		hash_map<ActorSharedPtr,int> map(mnet->get_actors()->size());
		int i=0;
		for (ActorSharedPtr actor: *mnet->get_actors()) {
			map[actor] = i;
			i++;
		}


		bool has_directed_layers = false;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			if (mnet->is_directed(layer,layer)) {
				has_directed_layers = true;
				break;
			}
		}
		if (has_directed_layers) {
		outfile << "#LAYERS" << std::endl;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			outfile << layer->name << sep << (mnet->is_directed(layer,layer)?"DIRECTED":"UNDIRECTED") << std::endl;
		}
		outfile << std::endl;
		}

		bool has_edge_attributes = false;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			if (mnet->edge_features(layer,layer)->attributes().size()>0){
				has_edge_attributes = true;
				break;
			}
		}
		if (has_edge_attributes) {
		outfile << "#EDGE ATTRIBUTES" << std::endl;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			for (AttributeSharedPtr attr: mnet->edge_features(layer,layer)->attributes()) {
				outfile << layer->name << sep << attr->name()<< sep << attr->type_as_string() << std::endl;
			}
		}
		outfile << std::endl;
		}

		if (has_directed_layers || has_edge_attributes)
			outfile << "#EDGES" << std::endl;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			for (EdgeSharedPtr edge: *mnet->get_edges(layer,layer)) {
					outfile << names.at(map.at(edge->v1->actor)) << sep << names.at(map.at(edge->v2->actor)) << sep << edge->v2->layer->name;
					AttributeStoreSharedPtr edge_attrs = mnet->edge_features(layer,layer);
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
		outfile.close();
	}


		void read_attributes(const AttributeStoreSharedPtr& store, object_id id, const vector<int>& attr_types, const vector<string>& attr_names, const vector<string>& line, int idx, int csv_line_number) {
			// Read node attributes
			if (idx+attr_names.size()>line.size())
				throw WrongFormatException("Line " + to_string(csv_line_number) + ": not enough attribute values");

			// indexes of current attribute in the local vectors and in the csv file.
			int attr_i=0;
			int csv_i=idx;
			for (string attr_name: attr_names) {
			switch (attr_types[attr_i]) {
				case NUMERIC_TYPE:
					store->setNumeric(id,attr_name,to_double(line[csv_i]));
					break;
				case STRING_TYPE:
					store->setString(id,attr_name,line[csv_i]);
					break;
				}
				attr_i++; csv_i++;
			}
		}

}
