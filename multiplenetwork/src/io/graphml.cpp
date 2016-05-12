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

void write_graphml(const MLNetworkSharedPtr& mnet, const string& path, const simple_set<LayerSharedPtr>& layers, bool merge_actors, bool include_all_actors) {

	std::ofstream outfile;
	outfile.open(path.data());

	outfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	outfile << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"" << std::endl;
	outfile << "    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""  << std::endl;
	outfile << "    xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns" << std::endl;
	outfile << "     http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">" << std::endl;

	// if there are directed edges, then the output graph will be directed
	// and undirected edges will be split into pairs of directed edges
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
				outfile << "    <key id=\"" << layer->name << ":" << attr->name() << "\" for=\"node\" attr.name=\"" << layer->name << ":" << attr->name() << "\" attr.type=\"double\"/>" << std::endl;
			else if (attr->type()==STRING_TYPE)
				outfile << "    <key id=\"" << layer->name << ":" << attr->name() << "\" for=\"node\" attr.name=\"" << layer->name << ":" << attr->name() << "\" attr.type=\"string\"/>" << std::endl;
		}
	}
	outfile << "    <key id=\"v_name\" for=\"node\" attr.name=\"name\" attr.type=\"string\"/>" << std::endl;
	outfile << "    <key id=\"e_type\" for=\"edge\" attr.name=\"e_type\" attr.type=\"string\"/>" << std::endl;

	// Edge attributes
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
	if (merge_actors) {
		// one for each actor
		for (ActorSharedPtr actor: mnet->get_actors()) {
			// except if only layer-specific actors must be used
			if (!include_all_actors) {
				bool is_in_input_layers = false;
				for (LayerSharedPtr layer: layers) {
					if (mnet->get_node(actor,layer))
						is_in_input_layers = true;
				}
				if (!is_in_input_layers) {
					continue;
				}
			}
			outfile << "    <node id=\"" << actor->id << "\">" << std::endl;
			outfile << "        <data key=\"v_name\">" << actor->name << "</data>" << std::endl;

			for (NodeSharedPtr node: mnet->get_nodes(actor)) {
				if (layers.count(node->layer)==0) {
					// no content
				}
				else {
					outfile << "        <data key=\"" << node->layer->name << "\">T</data>" << std::endl;
					AttributeStoreSharedPtr attrs = mnet->node_features(node->layer);
					for (AttributeSharedPtr attr: attrs->attributes()) {
						if (attr->type()==NUMERIC_TYPE)
							outfile << "        <data key=\"" << node->layer->name << ":" << attr->name() << "\">" << attrs->getNumeric(node->id,attr->name()) << "</data>" << std::endl;
						else if (attr->type()==STRING_TYPE)
							outfile << "        <data key=\"" << node->layer->name << ":" << attr->name() << "\">" << attrs->getString(node->id,attr->name()) << "</data>" << std::endl;
					}
				}
			}
			outfile << "    </node>" << std::endl;
		}
	}
	else {
		// No actor merging: one node for each node in the original multilayer network.
		// Only actors present in at least one layer are included: the include_all_actors parameter is not used in this case.
		for (LayerSharedPtr layer: layers) {
			for (NodeSharedPtr node: mnet->get_nodes(layer)) {
				outfile << "    <node id=\"" << node->id << "\">" << std::endl;
				outfile << "        <data key=\"v_name\">" << node->actor->name << ":" << node->layer->name << "</data>" << std::endl;
				AttributeStoreSharedPtr attrs = mnet->node_features(node->layer);
				for (AttributeSharedPtr attr: attrs->attributes()) {
					if (attr->type()==NUMERIC_TYPE)
						outfile << "        <data key=\"" << node->layer->name << ":" << attr->name() << "\">" << attrs->getNumeric(node->id,attr->name()) << "</data>" << std::endl;
					else if (attr->type()==STRING_TYPE)
						outfile << "        <data key=\"" << node->layer->name << ":" << attr->name() << "\">" << attrs->getString(node->id,attr->name()) << "</data>" << std::endl;
				}
				outfile << "    </node>" << std::endl;
			}
		}
	}

	outfile << "    <key id=\"e_type\" for=\"edge\" attr.name=\"e_type\" attr.type=\"string\"/>" << std::endl;

	// Edges
	if (merge_actors) {
		// connect actor ids
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
							outfile << "        <data key=\"e" << edge->v1->layer->name << "-" << edge->v2->layer->name << ": " << attr->name() << "\">" << attrs->getNumeric(edge->id,attr->name()) << "</data>" << std::endl;
						else if (attr->type()==STRING_TYPE)
							outfile << "        <data key=\"e" << edge->v1->layer->name << "-" << edge->v2->layer->name << ": " << attr->name() << "\">" << attrs->getString(edge->id,attr->name()) << "</data>" << std::endl;
					}
					outfile << "    </edge>" << std::endl;
				}
			}
		}
	}
	else {
		// connect node ids
		for (LayerSharedPtr layer1: layers) {
			for (LayerSharedPtr layer2: layers) {
				for (EdgeSharedPtr edge: mnet->get_edges(layer1,layer2)) {
					outfile << "    <edge id=\"e" << edge->id << "\" source=\"" << edge->v1->id << "\" target=\"" << edge->v2->id << "\">" << std::endl;
					outfile << "        <data key=\"e_type\">" << edge->v1->layer->name << "-" << edge->v1->layer->name << "</data>" << std::endl;
					AttributeStoreSharedPtr attrs = mnet->edge_features(edge->v1->layer,edge->v2->layer);
					for (AttributeSharedPtr attr: attrs->attributes()) {
						if (attr->type()==NUMERIC_TYPE)
							outfile << "        <data key=\"e" << edge->v1->layer->name << "-" << edge->v2->layer->name << ": " << attr->name() << "\">" << attrs->getNumeric(edge->id,attr->name()) << "</data>" << std::endl;
						else if (attr->type()==STRING_TYPE)
							outfile << "        <data key=\"e" << edge->v1->layer->name << "-" << edge->v2->layer->name << ": " << attr->name() << "\">" << attrs->getString(edge->id,attr->name()) << "</data>" << std::endl;
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

