#include "datastructures.h"

namespace mlnet {

MLNetwork::MLNetwork(const std::string& name) :
		name(name), max_node_id(0),
		max_edge_id(0), max_actor_id(0), max_layer_id(0) {
	layer_attributes = AttributeStoreSharedPtr(new AttributeStore());
	actor_attributes = AttributeStoreSharedPtr(new AttributeStore());
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
	actors.insert(aid,actor_ptr);
	cidx_actor_by_name[name] = actor_ptr;
	return actor_ptr;
}

ActorSharedPtr MLNetwork::get_actor(const actor_id& id) const {
	  return actors.get(id);
}

ActorSharedPtr MLNetwork::get_actor(const std::string& name) const {
	  if (cidx_actor_by_name.count(name)>0)
		  return cidx_actor_by_name.at(name);
	  else return NULL;
}

const sorted_set<actor_id,ActorSharedPtr>& MLNetwork::get_actors() const {
	return actors;
}

LayerSharedPtr MLNetwork::add_layer(const std::string& name, bool directed) {
	LayerSharedPtr check = get_layer(name);
	if (check) return NULL;
	layer_id id = ++max_layer_id;
	LayerSharedPtr layer_ptr(new layer(id,name));
	layers.insert(id,layer_ptr);
	cidx_layer_by_name[name] = layer_ptr;
	for (LayerSharedPtr layer : get_layers()) {
		sidx_edges_by_layer_pair[layer->id][layer_ptr->id]; // initialize an empty map
	}
	set_directed(layer_ptr,layer_ptr,directed);
	return layer_ptr;
}

void MLNetwork::set_directed(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2, bool directed) {
	edge_directionality[layer1->id][layer2->id] = directed;
	edge_directionality[layer2->id][layer1->id] = directed;
}

bool MLNetwork::is_directed(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) const {
	if (edge_directionality.count(layer1->id)>0) {
		if (edge_directionality.at(layer1->id).count(layer2->id)>0) {
			return edge_directionality.at(layer1->id).at(layer2->id);
		}
	}
	// else
	return DEFAULT_EDGE_DIRECTIONALITY;
}

LayerSharedPtr MLNetwork::get_layer(const layer_id& id) const {
	  return layers.get(id);
}

LayerSharedPtr MLNetwork::get_layer(const std::string& name) const {
	if (cidx_layer_by_name.count(name)>0) {
		return cidx_layer_by_name.at(name);
	}
	else {
		return NULL;
	}
}

const sorted_set<layer_id,LayerSharedPtr>& MLNetwork::get_layers() const {
	return layers;
}

NodeSharedPtr MLNetwork::add_node(const ActorSharedPtr& actor, const LayerSharedPtr& layer) {
	NodeSharedPtr check = get_node(actor,layer);
	if (check) return NULL;
	node_id id = ++max_node_id;
	NodeSharedPtr node_ptr(new node(id,actor,layer));
	nodes.insert(id,node_ptr);
	sidx_nodes_by_layer[layer->id].insert(id,node_ptr);
	cidx_node_by_actor_and_layer[actor->id][layer->id] = node_ptr;
	sidx_nodes_by_actor[actor->id].insert(id,node_ptr);
	return node_ptr;
}

NodeSharedPtr MLNetwork::get_node(const node_id& id) const {
	  return nodes.get(id);
}

NodeSharedPtr MLNetwork::get_node(const ActorSharedPtr& actor, const LayerSharedPtr& layer) const  {
	if (cidx_node_by_actor_and_layer.count(actor->id)>0 && cidx_node_by_actor_and_layer.at(actor->id).count(layer->id)>0) {
		return cidx_node_by_actor_and_layer.at(actor->id).at(layer->id);
	}
	else return NULL;
}

const sorted_set<node_id,NodeSharedPtr>& MLNetwork::get_nodes() const  {
	return nodes;
}

const sorted_set<node_id,NodeSharedPtr>& MLNetwork::get_nodes(const LayerSharedPtr& layer) const {
	if (sidx_nodes_by_layer.count(layer->id)==0)
		return *(new sorted_set<node_id,NodeSharedPtr>());
	return sidx_nodes_by_layer.at(layer->id);
}

const sorted_set<node_id,NodeSharedPtr>& MLNetwork::get_nodes(const ActorSharedPtr& actor) const {
	if (sidx_nodes_by_actor.count(actor->id)==0)
			return *(new sorted_set<node_id,NodeSharedPtr>());
		return sidx_nodes_by_actor.at(actor->id);
}

EdgeSharedPtr MLNetwork::add_edge(const NodeSharedPtr& node1, const NodeSharedPtr& node2) {
	EdgeSharedPtr check = get_edge(node1,node2);
	if (check) return NULL;
	edge_id eid = ++max_edge_id;
	bool edge_directed = is_directed(node1->layer,node2->layer);
	EdgeSharedPtr new_edge_v1_v2(new edge(eid,node1,node2,edge_directed));
	cidx_edge_by_nodes[node1->id][node2->id] = new_edge_v1_v2;
	sidx_neighbors_out[node1->id].insert(node2->id,node2);
	sidx_neighbors_in[node2->id].insert(node1->id,node1);
	sidx_neighbors_all[node1->id].insert(node2->id,node2);
	sidx_neighbors_all[node2->id].insert(node1->id,node1); // if node1 and node2 are the same, it also works
	sidx_edges_by_layer_pair[node1->layer->id][node2->layer->id].insert(eid,new_edge_v1_v2);
	edges.insert(eid,new_edge_v1_v2);
	if (!edge_directed) {
		// create an additional edge with the same identifier but inverted nodes
		EdgeSharedPtr new_edge_v2_v1(new edge(eid,node2,node1,edge_directed));
		cidx_edge_by_nodes[node2->id][node1->id] = new_edge_v2_v1;
		sidx_neighbors_out[node2->id].insert(node1->id,node1);
		sidx_neighbors_in[node1->id].insert(node2->id,node2);
		if (node1->layer->id!=node2->layer->id)
			sidx_edges_by_layer_pair[node2->layer->id][node1->layer->id].insert(eid,new_edge_v2_v1);
	}
	return new_edge_v1_v2;
}

EdgeSharedPtr MLNetwork::get_edge(const NodeSharedPtr& node1, const NodeSharedPtr& node2) const {
	if (cidx_edge_by_nodes.count(node1->id)>0 &&
		cidx_edge_by_nodes.at(node1->id).count(node2->id)>0)
		return cidx_edge_by_nodes.at(node1->id).at(node2->id);
	else return NULL;
}

const sorted_set<edge_id,EdgeSharedPtr>& MLNetwork::get_edges() const {
	return edges;
}

const sorted_set<edge_id,EdgeSharedPtr>& MLNetwork::get_edges(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) const {
	if (sidx_edges_by_layer_pair.count(layer1->id)==0 || sidx_edges_by_layer_pair.at(layer1->id).count(layer2->id)==0)
		return *(new sorted_set<edge_id,EdgeSharedPtr>());
	return sidx_edges_by_layer_pair.at(layer1->id).at(layer2->id);
}

bool MLNetwork::erase(const NodeSharedPtr& node) {
	// removing the node
	bool res = nodes.erase(node->id);
	sidx_nodes_by_layer[node->layer->id].erase(node->id);
	cidx_node_by_actor_and_layer[node->actor->id].erase(node->layer->id);

	// removing adjacent edges
	sorted_set<node_id,NodeSharedPtr> in = neighbors(node,IN);
	std::vector<EdgeSharedPtr> to_erase_in;
	to_erase_in.reserve(in.size());
	for (NodeSharedPtr node_in : in) {
		to_erase_in.push_back(get_edge(node_in,node));
	}
	for (EdgeSharedPtr edge : to_erase_in) {
		erase(edge);
	}
	sorted_set<node_id,NodeSharedPtr> out = neighbors(node,OUT);
	std::vector<EdgeSharedPtr> to_erase_out;
	to_erase_out.reserve(out.size());
	for (NodeSharedPtr node_out : out) {
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
	bool res = edges.erase(edge->id);
	sidx_edges_by_layer_pair[edge->v1->layer->id][edge->v2->layer->id].erase(edge->id);
	cidx_edge_by_nodes[edge->v1->id].erase(edge->v2->id);
	sidx_neighbors_in[edge->v2->id].erase(edge->v1->id);
	sidx_neighbors_out[edge->v1->id].erase(edge->v2->id);
	if (edge->directed && !get_edge(edge->v2,edge->v1)) {
		// if the edge is directed, we remove neighbors only if there isn't
		// an edge in the other direction keeping them neighbors
		sidx_neighbors_all[edge->v2->id].erase(edge->v1->id);
		sidx_neighbors_all[edge->v1->id].erase(edge->v2->id);
	}
	if (!edge->directed) {
		if (edge->v1->layer->id!=edge->v2->layer->id)
			sidx_edges_by_layer_pair[edge->v2->layer->id][edge->v1->layer->id].erase(edge->id);
		cidx_edge_by_nodes[edge->v2->id].erase(edge->v1->id);
		sidx_neighbors_in[edge->v1->id].erase(edge->v2->id);
		sidx_neighbors_out[edge->v2->id].erase(edge->v1->id);
		sidx_neighbors_all[edge->v1->id].erase(edge->v2->id);
		sidx_neighbors_all[edge->v2->id].erase(edge->v1->id);
	}
	edge_features(edge->v1->layer,edge->v2->layer)->reset(edge->id);
	return res;
}


bool MLNetwork::erase(const ActorSharedPtr& actor) {
	bool res = actors.erase(actor->id);
	cidx_actor_by_name.erase(actor->name);
	cidx_node_by_actor_and_layer.erase(actor->id);
	if (sidx_nodes_by_actor.count(actor->id)>0) {
		for (NodeSharedPtr node: sidx_nodes_by_actor.at(actor->id)) {
			erase(node);
		}
	}
	sidx_nodes_by_actor.erase(actor->id);
	actor_features()->reset(actor->id);
	return res;
}

bool MLNetwork::erase(const LayerSharedPtr& layer) {
	bool res = layers.erase(layer->id);
	if (sidx_nodes_by_layer.count(layer->id)>0) {
		std::vector<NodeSharedPtr> to_erase;
		to_erase.reserve(sidx_nodes_by_layer.at(layer->id).size());
		for (NodeSharedPtr node: sidx_nodes_by_layer.at(layer->id)) {
			to_erase.push_back(node);
		}
		for (NodeSharedPtr node: to_erase) {
			erase(node);
		}
	} // This automatically deletes all edges among nodes on that layer
	layer_features()->reset(layer->id);
	return res;
}

const sorted_set<node_id,NodeSharedPtr>& MLNetwork::neighbors(const NodeSharedPtr& node, edge_mode mode) const {
	if (mode==IN) {
		if (sidx_neighbors_in.count(node->id)==0) {
			return empty;
		}
		return sidx_neighbors_in.at(node->id);
	}
	else if (mode==OUT) {
		if (sidx_neighbors_out.count(node->id)==0) {
			return empty;
		}
		return sidx_neighbors_out.at(node->id);
	}
	else if (mode==INOUT) {
		if (sidx_neighbors_all.count(node->id)==0) {
			return empty;
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
		node_attributes[layer->id] = AttributeStoreSharedPtr(new AttributeStore());
	}
	return node_attributes[layer->id];
}

const AttributeStoreSharedPtr MLNetwork::node_features(const LayerSharedPtr& layer) const {
	if (node_attributes.count(layer->id)==0) {
		return AttributeStoreSharedPtr(new AttributeStore());
	}
	return node_attributes.at(layer->id);
}

// layer-specific attribute stores are initialized if needed
AttributeStoreSharedPtr MLNetwork::edge_features(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	if (edge_attributes.count(layer1->id)==0 || edge_attributes.at(layer1->id).count(layer2->id)==0) {
		edge_attributes[layer1->id][layer2->id] = AttributeStoreSharedPtr(new AttributeStore());
		if (!is_directed(layer1,layer2))
			edge_attributes[layer2->id][layer1->id] = edge_attributes[layer1->id][layer2->id];
	}
	return edge_attributes[layer1->id][layer2->id];
}

const AttributeStoreSharedPtr MLNetwork::edge_features(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) const {
	if (edge_attributes.count(layer1->id)==0 || edge_attributes.at(layer1->id).count(layer2->id)==0) {
		return AttributeStoreSharedPtr(new AttributeStore());
	}
	return edge_attributes.at(layer1->id).at(layer2->id);
}

std::string MLNetwork::to_string() const {
	return "Multilayer Network (\"" + name + "\": " + std::to_string(get_layers().size()) + " layers, " +
			std::to_string(get_actors().size()) + " actors, " +
			std::to_string(get_nodes().size()) + " nodes, " +
			std::to_string(get_edges().size()) + " edges)";
}

} // namespace mlnet
