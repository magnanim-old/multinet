#include "datastructures.h"

namespace mlnet {

MLNetwork::MLNetwork(const std::string& name) :
		name(name), max_node_id(0),
		max_edge_id(0), max_actor_id(0), max_layer_id(0) {
	actors = std::make_shared<actor_list>();
	layers = std::make_shared<layer_list>();
	nodes = std::make_shared<node_list>();
	edges = std::make_shared<edge_list>();
	empty_nodes = std::make_shared<node_list>();
	empty_edges = std::make_shared<edge_list>();
	layer_attributes = AttributeStore::create();
	actor_attributes = AttributeStore::create();
}

MLNetworkSharedPtr MLNetwork::create(const std::string& name) {
	MLNetworkSharedPtr result(new MLNetwork(name));
	return result;
}

ActorSharedPtr MLNetwork::add_actor(const std::string& name)  {
	ActorSharedPtr check = get_actor(name);
	if (check) return NULL;
	actor_id aid = ++max_actor_id;
	ActorSharedPtr actor_ptr(new actor(aid,name));
	actors->insert(actor_ptr);
	cidx_actor_by_name[name] = actor_ptr;
	return actor_ptr;
}

ActorSharedPtr MLNetwork::get_actor(const std::string& name) const {
	  if (cidx_actor_by_name.count(name)>0)
		  return cidx_actor_by_name.at(name);
	  else return NULL;
}

const ActorListSharedPtr MLNetwork::get_actors() const {
	return actors;
}

LayerSharedPtr MLNetwork::add_layer(const std::string& name, edge_directionality directionality) {
	LayerSharedPtr check = get_layer(name);
	if (check) return NULL;
	layer_id id = ++max_layer_id;
	LayerSharedPtr layer_ptr(new layer(id,name));
	layers->insert(layer_ptr);
	cidx_layer_by_name[name] = layer_ptr;
	switch (directionality) {
	case DIRECTED: set_directed(layer_ptr,layer_ptr,true); break;
	case UNDIRECTED: set_directed(layer_ptr,layer_ptr,false); break;
	}
	return layer_ptr;
}

void MLNetwork::set_directed(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2, bool directed) {
	_edge_directionality[layer1->id][layer2->id] = directed;
	_edge_directionality[layer2->id][layer1->id] = directed;
}

bool MLNetwork::is_directed(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) const {
	if (_edge_directionality.count(layer1->id)>0) {
		if (_edge_directionality.at(layer1->id).count(layer2->id)>0) {
			return _edge_directionality.at(layer1->id).at(layer2->id);
		}
	}
	// else
	return DEFAULT_EDGE_DIRECTIONALITY;
}

/*LayerSharedPtr MLNetwork::get_layer(layer_id id) const {
	  return layers->get(id);
}*/

LayerSharedPtr MLNetwork::get_layer(const std::string& name) const {
	if (cidx_layer_by_name.count(name)>0) {
		return cidx_layer_by_name.at(name);
	}
	else {
		return NULL;
	}
}

const LayerListSharedPtr MLNetwork::get_layers() const {
	return layers;
}

NodeSharedPtr MLNetwork::add_node(const ActorSharedPtr& actor, const LayerSharedPtr& layer) {
	NodeSharedPtr check = get_node(actor,layer);
	if (check) return NULL;
	node_id id = ++max_node_id;
	NodeSharedPtr node_ptr(new node(id,actor,layer));
	nodes->insert(node_ptr);
	if (sidx_nodes_by_layer.count(layer->id)==0)
		sidx_nodes_by_layer[layer->id] = std::make_shared<node_list>();
	sidx_nodes_by_layer[layer->id]->insert(node_ptr);
	cidx_node_by_actor_and_layer[actor->id][layer->id] = node_ptr;
	if (sidx_nodes_by_actor.count(actor->id)==0)
		sidx_nodes_by_actor[actor->id] = std::make_shared<node_list>();
	sidx_nodes_by_actor[actor->id]->insert(node_ptr);
	return node_ptr;
}

NodeSharedPtr MLNetwork::get_node(const ActorSharedPtr& actor, const LayerSharedPtr& layer) const  {
	if (cidx_node_by_actor_and_layer.count(actor->id)>0 && cidx_node_by_actor_and_layer.at(actor->id).count(layer->id)>0) {
		return cidx_node_by_actor_and_layer.at(actor->id).at(layer->id);
	}
	else return NULL;
}

const NodeListSharedPtr MLNetwork::get_nodes() const  {
	return nodes;
}

const NodeListSharedPtr MLNetwork::get_nodes(const LayerSharedPtr& layer) const {
	if (sidx_nodes_by_layer.count(layer->id)==0)
		return empty_nodes;
	return sidx_nodes_by_layer.at(layer->id);
}

const NodeListSharedPtr MLNetwork::get_nodes(const ActorSharedPtr& actor) const {
	if (sidx_nodes_by_actor.count(actor->id)==0)
			return empty_nodes;
		return sidx_nodes_by_actor.at(actor->id);
}

EdgeSharedPtr MLNetwork::add_edge(const NodeSharedPtr& node1, const NodeSharedPtr& node2) {
	EdgeSharedPtr check = get_edge(node1,node2);
	if (check) return NULL;
	edge_id eid = ++max_edge_id;
	edge_directionality directionality = is_directed(node1->layer,node2->layer)?DIRECTED:UNDIRECTED;
	EdgeSharedPtr new_edge_v1_v2(new edge(eid,node1,node2,directionality));
	cidx_edge_by_nodes[node1->id][node2->id] = new_edge_v1_v2;
	if (sidx_neighbors_out.count(node1->id)==0)
		sidx_neighbors_out[node1->id] = std::make_shared<node_list>();
	sidx_neighbors_out[node1->id]->insert(node2);
	if (sidx_neighbors_in.count(node2->id)==0)
		sidx_neighbors_in[node2->id] = std::make_shared<node_list>();
	sidx_neighbors_in[node2->id]->insert(node1);
	if (sidx_neighbors_all.count(node1->id)==0)
		sidx_neighbors_all[node1->id] = std::make_shared<node_list>();
	sidx_neighbors_all[node1->id]->insert(node2);
	if (sidx_neighbors_all.count(node2->id)==0)
		sidx_neighbors_all[node2->id] = std::make_shared<node_list>();
	sidx_neighbors_all[node2->id]->insert(node1); // if node1 and node2 are the same, it also works
	if (sidx_edges_by_layer_pair[node1->layer->id].count(node2->layer->id)==0)
		sidx_edges_by_layer_pair[node1->layer->id][node2->layer->id] = std::make_shared<edge_list>();
	sidx_edges_by_layer_pair[node1->layer->id][node2->layer->id]->insert(new_edge_v1_v2);
	edges->insert(new_edge_v1_v2);
	if (directionality==UNDIRECTED) {
		cidx_edge_by_nodes[node2->id][node1->id] = new_edge_v1_v2;
		if (sidx_neighbors_out.count(node2->id)==0)
			sidx_neighbors_out[node2->id] = std::make_shared<node_list>();
		sidx_neighbors_out[node2->id]->insert(node1);
		if (sidx_neighbors_in.count(node1->id)==0)
			sidx_neighbors_in[node1->id] = std::make_shared<node_list>();
		sidx_neighbors_in[node1->id]->insert(node2);
		if (node1->layer->id!=node2->layer->id) {
			if (sidx_edges_by_layer_pair[node2->layer->id].count(node1->layer->id)==0)
				sidx_edges_by_layer_pair[node2->layer->id][node1->layer->id] = std::make_shared<edge_list>();
		}
		sidx_edges_by_layer_pair[node2->layer->id][node1->layer->id]->insert(new_edge_v1_v2);
	}
	return new_edge_v1_v2;
}

EdgeSharedPtr MLNetwork::get_edge(const NodeSharedPtr& node1, const NodeSharedPtr& node2) const {
	if (cidx_edge_by_nodes.count(node1->id)>0 &&
		cidx_edge_by_nodes.at(node1->id).count(node2->id)>0)
		return cidx_edge_by_nodes.at(node1->id).at(node2->id);
	else return NULL;
}

const EdgeListSharedPtr MLNetwork::get_edges() const {
	return edges;
}

const EdgeListSharedPtr MLNetwork::get_edges(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) const {
	if (sidx_edges_by_layer_pair.count(layer1->id)==0 || sidx_edges_by_layer_pair.at(layer1->id).count(layer2->id)==0)
		return empty_edges;
	return sidx_edges_by_layer_pair.at(layer1->id).at(layer2->id);
}

bool MLNetwork::erase(const NodeSharedPtr& node) {
	// removing the node
	bool res = nodes->erase(node);
    sidx_nodes_by_layer[node->layer->id]->erase(node);
    sidx_nodes_by_actor[node->actor->id]->erase(node);
	cidx_node_by_actor_and_layer[node->actor->id].erase(node->layer->id);

	// removing adjacent edges
	NodeListSharedPtr in = neighbors(node,IN);
	vector<EdgeSharedPtr> to_erase_in;
	to_erase_in.reserve(in->size());
	for (NodeSharedPtr node_in : *in) {
		to_erase_in.push_back(get_edge(node_in,node));
	}
	for (EdgeSharedPtr edge : to_erase_in) {
		erase(edge);
	}
	NodeListSharedPtr out = neighbors(node,OUT);
	vector<EdgeSharedPtr> to_erase_out;
	to_erase_out.reserve(out->size());
	for (NodeSharedPtr node_out : *out) {
		to_erase_out.push_back(get_edge(node,node_out));
	}
	for (EdgeSharedPtr edge : to_erase_out) {
		erase(edge);
	}
	// remove attribute values
	node_features(node->layer)->reset(node->id);
	return res;
}

bool MLNetwork::erase(const EdgeSharedPtr& edge) {
	bool res = edges->erase(edge);
	sidx_edges_by_layer_pair[edge->v1->layer->id][edge->v2->layer->id]->erase(edge);
	cidx_edge_by_nodes[edge->v1->id].erase(edge->v2->id);
	sidx_neighbors_in[edge->v2->id]->erase(edge->v1);
	sidx_neighbors_out[edge->v1->id]->erase(edge->v2);
	if (edge->directionality && !get_edge(edge->v2,edge->v1)) {
		// if the edge is directed, we remove neighbors only if there isn't
		// an edge in the other direction keeping them neighbors
		sidx_neighbors_all[edge->v2->id]->erase(edge->v1);
		sidx_neighbors_all[edge->v1->id]->erase(edge->v2);
	}
	if (!edge->directionality) {
		if (edge->v1->layer->id!=edge->v2->layer->id)
			sidx_edges_by_layer_pair[edge->v2->layer->id][edge->v1->layer->id]->erase(edge);
		cidx_edge_by_nodes[edge->v2->id].erase(edge->v1->id);
		sidx_neighbors_in[edge->v1->id]->erase(edge->v2);
		sidx_neighbors_out[edge->v2->id]->erase(edge->v1);
		sidx_neighbors_all[edge->v1->id]->erase(edge->v2);
		sidx_neighbors_all[edge->v2->id]->erase(edge->v1);
	}
	edge_features(edge->v1->layer,edge->v2->layer)->reset(edge->id);
	return res;
}


bool MLNetwork::erase(const ActorSharedPtr& actor) {
	bool res = actors->erase(actor);
	cidx_actor_by_name.erase(actor->name);
	cidx_node_by_actor_and_layer.erase(actor->id);
	if (sidx_nodes_by_actor.count(actor->id)>0) {
		for (NodeSharedPtr node: *sidx_nodes_by_actor.at(actor->id)) {
			erase(node);
		}
	}
	sidx_nodes_by_actor.erase(actor->id);
	actor_features()->reset(actor->id);
	return res;
}

bool MLNetwork::erase(const LayerSharedPtr& layer) {
	bool res = layers->erase(layer);
    cidx_layer_by_name.erase(layer->name);
	if (sidx_nodes_by_layer.count(layer->id)>0) {
		vector<NodeSharedPtr> to_erase;
		to_erase.reserve(sidx_nodes_by_layer.at(layer->id)->size());
		for (NodeSharedPtr node: *sidx_nodes_by_layer.at(layer->id)) {
			to_erase.push_back(node);
		}
		for (NodeSharedPtr node: to_erase) {
			erase(node);
		}
	} // This automatically deletes all edges among nodes on that layer
	layer_features()->reset(layer->id);
	return res;
}

const NodeListSharedPtr MLNetwork::neighbors(const NodeSharedPtr& node, edge_mode mode) const {
	if (mode==IN) {
		if (sidx_neighbors_in.count(node->id)==0) {
			return empty_nodes;
		}
		return sidx_neighbors_in.at(node->id);
	}
	else if (mode==OUT) {
		if (sidx_neighbors_out.count(node->id)==0) {
			return empty_nodes;
		}
		return sidx_neighbors_out.at(node->id);
	}
	else if (mode==INOUT) {
		if (sidx_neighbors_all.count(node->id)==0) {
			return empty_nodes;
		}
		return sidx_neighbors_all.at(node->id);
	}
	else throw WrongParameterException("neighborhood mode");
}

// this attribute store is initialized in the constructor
AttributeStoreSharedPtr MLNetwork::actor_features() {
	return actor_attributes;
}

// this attribute store is initialized in the constructor
const AttributeStoreSharedPtr MLNetwork::actor_features() const {
	return actor_attributes;
}

// this attribute store is initialized in the constructor
AttributeStoreSharedPtr MLNetwork::layer_features() {
	return layer_attributes;
}

// this attribute store is initialized in the constructor
const AttributeStoreSharedPtr MLNetwork::layer_features() const {
	return layer_attributes;
}

// layer-specific attribute stores are initialized if needed
AttributeStoreSharedPtr MLNetwork::node_features(const LayerSharedPtr& layer) {
	if (node_attributes.count(layer->id)==0) {
		node_attributes[layer->id] = AttributeStore::create();
	}
	return node_attributes[layer->id];
}

const AttributeStoreSharedPtr MLNetwork::node_features(const LayerSharedPtr& layer) const {
	if (node_attributes.count(layer->id)==0) {
		return AttributeStore::create();
	}
	return node_attributes.at(layer->id);
}

// layer-specific attribute stores are initialized if needed
AttributeStoreSharedPtr MLNetwork::edge_features(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	if (edge_attributes.count(layer1->id)==0 || edge_attributes.at(layer1->id).count(layer2->id)==0) {
		edge_attributes[layer1->id][layer2->id] = AttributeStore::create();
		if (!is_directed(layer1,layer2))
			edge_attributes[layer2->id][layer1->id] = edge_attributes[layer1->id][layer2->id];
	}
	return edge_attributes[layer1->id][layer2->id];
}

const AttributeStoreSharedPtr MLNetwork::edge_features(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) const {
	if (edge_attributes.count(layer1->id)==0 || edge_attributes.at(layer1->id).count(layer2->id)==0) {
		return AttributeStore::create();
	}
	return edge_attributes.at(layer1->id).at(layer2->id);
}

void MLNetwork::set_weight(const NodeSharedPtr& node1, const NodeSharedPtr& node2, double weight) {
	EdgeSharedPtr edge = get_edge(node1,node2);
	if (!edge) throw ElementNotFoundException("edge between " + node1->to_string() + " and " + node2->to_string());
	edge_features(node1->layer,node2->layer)->setNumeric(edge->id,DEFAULT_WEIGHT_ATTR_NAME,weight);
}

double MLNetwork::get_weight(const NodeSharedPtr& node1, const NodeSharedPtr& node2) {
	EdgeSharedPtr edge = get_edge(node1,node2);
	if (!edge) throw ElementNotFoundException("edge between " + node1->to_string() + " and " + node2->to_string());
	return edge_features(node1->layer,node2->layer)->getNumeric(edge->id,DEFAULT_WEIGHT_ATTR_NAME);
}

std::string MLNetwork::to_string() const {
	std::string summary =
			"Multilayer Network (\"" + name + "\": " + std::to_string(get_layers()->size()) + " layers, " +
			std::to_string(get_actors()->size()) + " actors, " +
			std::to_string(get_nodes()->size()) + " nodes, " +
			std::to_string(get_edges()->size()) + " edges)";
	return summary;
}

std::string MLNetwork::to_long_string() const {
	std::string summary =
			"Multilayer Network (\"" + name + "\": " + std::to_string(get_layers()->size()) + " layers, " +
			std::to_string(get_actors()->size()) + " actors, " +
			std::to_string(get_nodes()->size()) + " nodes, " +
			std::to_string(get_edges()->size()) + " edges)\n";
	int num_attributes = 0;
	std::string attributes = "";
	for (AttributeSharedPtr a: layer_features()->attributes()) {
		attributes += "- " + a->name() + " (layer, " + a->type_as_string() + ")\n";
		num_attributes++;
	}
	for (AttributeSharedPtr a: actor_features()->attributes()) {
		attributes += "- " + a->name() + " (actor, " + a->type_as_string() + ")\n";
		num_attributes++;
	}
	for (LayerSharedPtr layer: *get_layers()) {
		for (AttributeSharedPtr a: node_features(layer)->attributes()) {
			attributes += "- " + a->name() + " (node, layer: " + layer->name + ", " + a->type_as_string() + ")\n";
			num_attributes++;
		}
	}
	for (LayerSharedPtr layer: *get_layers()) {
		for (AttributeSharedPtr a: edge_features(layer,layer)->attributes()) {
			attributes += "- " + a->name() + " (edge, layer: " + layer->name + ", " + a->type_as_string() + ")\n";
			num_attributes++;
		}
	}
	for (LayerSharedPtr layer1: *get_layers()) {
		for (LayerSharedPtr layer2: *get_layers()) {
			if (layer1==layer2) continue;
			for (AttributeSharedPtr a: edge_features(layer1,layer2)->attributes()) {
				attributes += "- " + a->name() + " (edge, layers " + layer1->name + "--" + layer2->name + ", " + a->type_as_string() + ")\n";
				num_attributes++;
			}
		}
	}
	if (num_attributes>0)
		summary = summary + "Attributes:\n" + attributes;
	return summary;
}

} // namespace mlnet
