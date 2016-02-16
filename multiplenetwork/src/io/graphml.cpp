/*
 * creation.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: magnanim
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include "io.h"
#include "utils.h"
#include "datastructures.h"
#include "exceptions.h"

using namespace std;

namespace mlnet {

void write_graphml(const MLNetworkSharedPtr mnet, const std::unordered_set<LayerSharedPtr>& layers, const std::string& path, bool multigraph) {
	std::ofstream outfile;
	outfile.open(path.data());

	outfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	outfile << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"" << std::endl;
	outfile << "    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""  << std::endl;
	outfile << "    xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns" << std::endl;
	outfile << "     http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">" << std::endl;

	bool directed = false;
	for (LayerSharedPtr layer1: layers) {
		for (LayerSharedPtr layer2: layers) {
			if (mnet->is_directed(layer1,layer2)) {
				directed = true;
				goto end_loop;
			}
		}
	}
	end_loop:

	// Node attributes
	for (LayerSharedPtr layer: layers) {
		outfile << "    <key id=\"l" << layer->id << "\" for=\"node\" attr.name=\"" << layer->name << "\" attr.type=\"string\"/>" << std::endl;
		for (AttributeSharedPtr attr: mnet->node_features(layer)->attributes()) {
			if (attr->type()==NUMERIC_TYPE)
				outfile << "    <key id=\"l" << layer->id << ":" << attr->name() << "\" for=\"node\" attr.name=\"" << layer->name << ":" << attr->name() << "\" attr.type=\"double\"/>" << std::endl;
			else if (attr->type()==STRING_TYPE)
				outfile << "    <key id=\"l" << layer->id << ":" << attr->name() << "\" for=\"node\" attr.name=\"" << layer->name << ":" << attr->name() << "\" attr.type=\"string\"/>" << std::endl;
		}
	}
	outfile << "    <key id=\"e_type\" for=\"edge\" attr.name=\"e_type\" attr.type=\"string\"/>" << std::endl;

	for (LayerSharedPtr layer1: layers) {
		for (LayerSharedPtr layer2: layers) {
			for (AttributeSharedPtr attr: mnet->edge_features(layer1,layer2)->attributes()) {
				if (attr->type()==NUMERIC_TYPE)
					outfile << "    <key id=\"e" << layer1->name << "-" << layer2->name << ": " << attr->name() << "\" for=\"edge\" attr.name=\"" << layer1->name << "-" << layer2->name << ": "  << attr->name() << "\" attr.type=\"double\"/>" << std::endl;
				else if (attr->type()==STRING_TYPE)
					outfile << "    <key id=\"e" << layer1->name << "-" << layer2->name << ": " << attr->name() << "\" for=\"edge\" attr.name=\"" << layer1->name << "-" << layer2->name << ": " << attr->name() << "\" attr.type=\"string\"/>" << std::endl;
			}
		}
	}

	outfile << "  <graph id=\"" << mnet->name << "\" edgedefault=\"" << (directed?"directed":"undirected") << "\">" << std::endl;

	// Nodes
	if (multigraph) {
		// (one for each actor in the input layers)
		for (ActorSharedPtr actor: mnet->get_actors()) {
			bool printed = false;
			for (NodeSharedPtr node: mnet->get_nodes(actor)) {
				if (!printed) {
					outfile << "    <node id=\"" << actor->id << "\" name=\"" << actor->name << "\">" << std::endl;
					printed = true;
				}
				if (layers.count(node->layer)==0)
					continue;
				outfile << "        <data key=\"l" << node->layer->id << "\">T</data>" << std::endl;
				AttributeStoreSharedPtr attrs = mnet->node_features(node->layer);
				for (AttributeSharedPtr attr: attrs->attributes()) {
					if (attr->type()==NUMERIC_TYPE)
						outfile << "        <data key=\"" << node->layer->name << ":" << attr->name() << "\">" << attrs->getNumeric(node->id,attr->name()) << "</data>" << std::endl;
					else if (attr->type()==STRING_TYPE)
						outfile << "        <data key=\"" << node->layer->name << ":" << attr->name() << "\">" << attrs->getString(node->id,attr->name()) << "</data>" << std::endl;
				}
			}
			if (printed)
				outfile << "    </node>" << std::endl;
		}
	}
	else {
		// (one for each node)
		for (LayerSharedPtr layer: layers) {
			for (NodeSharedPtr node: mnet->get_nodes(layer)) {
				outfile << "    <node id=\"" << node->id << "\" name=\"" << node->actor->name << ":" << node->layer->name << "\">" << std::endl;
				AttributeStoreSharedPtr attrs = mnet->node_features(node->layer);
				for (AttributeSharedPtr attr: attrs->attributes()) {
					if (attr->type()==NUMERIC_TYPE)
						outfile << "        <data key=\"v" << node->layer->name << ": " << attr->name() << "\">" << attrs->getNumeric(node->id,attr->name()) << "</data>" << std::endl;
					else if (attr->type()==STRING_TYPE)
						outfile << "        <data key=\"v" << node->layer->name << ": " << attr->name() << "\">" << attrs->getString(node->id,attr->name()) << "</data>" << std::endl;
				}
				outfile << "    </node>" << std::endl;
			}
		}
	}

	outfile << "    <key id=\"e_type\" for=\"edge\" attr.name=\"e_type\" attr.type=\"string\"/>" << std::endl;

	// Edges
	if (multigraph) {
		// (connect actor ids)
		for (LayerSharedPtr layer1: layers) {
			for (LayerSharedPtr layer2: layers) {
				for (EdgeSharedPtr edge: mnet->get_edges(layer1,layer2)) {
					outfile << "    <edge id=\"e" << edge->id << "\" source=\"" << edge->v1->actor->id << "\" target=\"" << edge->v2->actor->id << "\">" << std::endl;
					if (edge->v1->layer==edge->v2->layer) {
						outfile << "        <data key=\"e_type\">" << edge->v1->layer->name << "</data>" << std::endl;
					}
					else
						outfile << "        <data key=\"e_type\">" << edge->v1->layer->name << "-" << edge->v2->layer->name << "</data>" << std::endl;
					AttributeStoreSharedPtr attrs = mnet->edge_features(edge->v1->layer,edge->v2->layer);
					for (AttributeSharedPtr attr: attrs->attributes()) {
						if (attr->type()==NUMERIC_TYPE)
							outfile << "        <data key=\"e" << edge->v1->layer->name << "-" << edge->v2->layer->name << "_" << "\">" << attrs->getNumeric(edge->id,attr->name()) << "</data>" << std::endl;
						else if (attr->type()==STRING_TYPE)
							outfile << "        <data key=\"e" << edge->v1->layer->name << "-" << edge->v2->layer->name << "_" << "\">" << attrs->getString(edge->id,attr->name()) << "</data>" << std::endl;
					}
					outfile << "    </edge>" << std::endl;
				}
			}
		}
	}
	else {
		// (connect node ids)
		for (LayerSharedPtr layer1: layers) {
			for (LayerSharedPtr layer2: layers) {
				for (EdgeSharedPtr edge: mnet->get_edges(layer1,layer2)) {
					outfile << "    <edge id=\"e" << edge->id << "\" source=\"" << edge->v1->id << "\" target=\"" << edge->v2->id << "\">" << std::endl;
					outfile << "        <data key=\"e_type\">" << edge->v1->layer->name << "-" << edge->v1->layer->name << "</data>" << std::endl;
					AttributeStoreSharedPtr attrs = mnet->edge_features(edge->v1->layer,edge->v2->layer);
					for (AttributeSharedPtr attr: attrs->attributes()) {
						if (attr->type()==NUMERIC_TYPE)
							outfile << "        <data key=\"e" << edge->v1->layer->name << "-" << edge->v2->layer->name << "_" << "\">" << attrs->getNumeric(edge->id,attr->name()) << "</data>" << std::endl;
						else if (attr->type()==STRING_TYPE)
							outfile << "        <data key=\"e" << edge->v1->layer->name << "-" << edge->v2->layer->name << "_" << "\">" << attrs->getString(edge->id,attr->name()) << "</data>" << std::endl;
					}
					outfile << "    </edge>" << std::endl;
				}
			}
		}
	}
	outfile << "  </graph>" << std::endl;
	outfile << "</graphml>" << std::endl;
}

}

