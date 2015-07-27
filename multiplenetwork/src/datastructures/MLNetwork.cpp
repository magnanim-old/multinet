#include "datastructures.h"
#include "utils.h"

namespace mlnet {

MLNetwork::MLNetwork(const std::string& name) :
		mlnet_name(name), max_node_id(0),
		max_edge_id(0), max_actor_id(0), max_layer_id(0) {
	layer_attributes = AttributeStoreSharedPtr(new AttributeStore());
	actor_attributes = AttributeStoreSharedPtr(new AttributeStore());
}

MLNetworkSharedPtr MLNetwork::create(const std::string& name) {
	MLNetworkSharedPtr result(new MLNetwork(name));
	return result;
}

MLNetwork::~MLNetwork() {}

std::string MLNetwork::name() const {
	return mlnet_name;
}

ActorSharedPtr MLNetwork::add_actor() {
	actor_id aid = ++max_actor_id;
	std::string name = "@"+std::to_string(aid);
	ActorSharedPtr actor_ptr(new actor(aid,name));
	actors_by_id.insert(aid,actor_ptr);
	actors_by_name[name] = actor_ptr;
	return actor_ptr;
}

ActorSharedPtr MLNetwork::add_actor(const std::string& name)  {
	ActorSharedPtr check = get_actor(name);
	if (check) throw DuplicateElementException("actor " + check->to_string());
	actor_id aid = ++max_actor_id;
	ActorSharedPtr actor_ptr(new actor(aid,name));
	actors_by_id.insert(aid,actor_ptr);
	actors_by_name[name] = actor_ptr;
	return actor_ptr;
}

ActorSharedPtr MLNetwork::get_actor(const actor_id& id) const {
	  return actors_by_id.get(id);
}

ActorSharedPtr MLNetwork::get_actor(const std::string& name) const {
	  try {return actors_by_name.at(name);}
	  catch (const std::out_of_range& oor) {return NULL;} // TODO write without exception, here and later
}

const ObjectStore<ActorSharedPtr>& MLNetwork::get_actors() const {
	return actors_by_id;
}

LayerSharedPtr MLNetwork::add_layer(bool directed) {
	layer_id id = ++max_layer_id;
	std::string name = "l"+std::to_string(id);
	LayerSharedPtr layer_ptr(new layer(id,name));
	layers_by_id.insert(id,layer_ptr);
	layers_by_name[name] = layer_ptr;
	set_directed(layer_ptr,layer_ptr,directed);
	return layer_ptr;
}

LayerSharedPtr MLNetwork::add_layer(const std::string& name, bool directed) {
	LayerSharedPtr check = get_layer(name);
	if (check) throw DuplicateElementException("layer " + check->to_string());
	layer_id id = ++max_layer_id;
	LayerSharedPtr layer_ptr(new layer(id,name));
	layers_by_id.insert(id,layer_ptr);
	layers_by_name[name] = layer_ptr;
	for (LayerSharedPtr layer : get_layers()) {
		edges_by_layers_and_id[layer->id][layer_ptr->id]; // initialize an empty map
	}
	set_directed(layer_ptr,layer_ptr,directed);
	return layer_ptr;
}

void MLNetwork::set_directed(LayerSharedPtr layer1, LayerSharedPtr layer2, bool directed) {
	edge_directionality[layer1->id][layer2->id] = directed;
	if (!directed) {
		edge_directionality[layer2->id][layer1->id] = directed;
	}
}

bool MLNetwork::is_directed(LayerSharedPtr layer1, LayerSharedPtr layer2) const {
	if (edge_directionality.count(layer1->id)>0) {
		if (edge_directionality.at(layer1->id).count(layer2->id)>0) {
			return edge_directionality.at(layer1->id).at(layer2->id);
		}
	}
	// else
	return DEFAULT_EDGE_DIRECTIONALITY;
}

LayerSharedPtr MLNetwork::get_layer(const layer_id& id) const {
	  return layers_by_id.get(id);
}

LayerSharedPtr MLNetwork::get_layer(const std::string& name) const {
	  try {return layers_by_name.at(name);}
	  catch (const std::out_of_range& oor) {return NULL;}
}

const ObjectStore<LayerSharedPtr>& MLNetwork::get_layers() const {
	return layers_by_id;
}

NodeSharedPtr MLNetwork::add_node(const ActorSharedPtr& actor, const LayerSharedPtr& layer) {
	node_id id = ++max_node_id;
	std::string name = "node"+std::to_string(id);
	NodeSharedPtr node_ptr(new node(id,name,actor,layer));
	nodes_by_id.insert(id,node_ptr);
	nodes_by_layer_and_id[layer->id].insert(id,node_ptr);
	nodes_by_layer_and_name[layer->id][name] = node_ptr;
	nodes_by_actor_and_id[actor->id].insert(id,node_ptr);
	return node_ptr;
}

NodeSharedPtr MLNetwork::add_node(const std::string& name, const ActorSharedPtr& actor, const LayerSharedPtr& layer) {
	NodeSharedPtr check = get_node(name, layer);
	if (check) throw DuplicateElementException("node " + name + " on layer " + layer->to_string());
	node_id id = ++max_node_id;
	NodeSharedPtr node_ptr(new node(id,name,actor,layer));
	nodes_by_id.insert(id,node_ptr);
	nodes_by_layer_and_id[layer->id].insert(id,node_ptr);
	nodes_by_layer_and_name[layer->id][name] = node_ptr;
	nodes_by_actor_and_id[actor->id].insert(id,node_ptr);
	return node_ptr;
}

NodeSharedPtr MLNetwork::get_node(const node_id& id) const {
	  return nodes_by_id.get(id);
}

NodeSharedPtr MLNetwork::get_node(const std::string& name, const LayerSharedPtr& layer) const  {
	  try {return nodes_by_layer_and_name.at(layer->id).at(name);}
	  catch (const std::out_of_range& oor) {return NULL;}
}

const ObjectStore<NodeSharedPtr>& MLNetwork::get_nodes() const  {
	return nodes_by_id;
}

const ObjectStore<NodeSharedPtr>& MLNetwork::get_nodes(const LayerSharedPtr& layer) const {
	if (nodes_by_layer_and_id.count(layer->id)==0)
		return *(new ObjectStore<NodeSharedPtr>());
	return nodes_by_layer_and_id.at(layer->id);
}

const ObjectStore<NodeSharedPtr>& MLNetwork::get_nodes(const ActorSharedPtr& actor) const {
	if (nodes_by_actor_and_id.count(actor->id)==0)
			return *(new ObjectStore<NodeSharedPtr>());
		return nodes_by_actor_and_id.at(actor->id);
}

EdgeSharedPtr MLNetwork::add_edge(const NodeSharedPtr& node1, const NodeSharedPtr& node2) {
	edge_id eid = ++max_edge_id;
	bool edge_directed = is_directed(node1->layer,node2->layer);
	EdgeSharedPtr new_edge_v1_v2(new edge(eid,node1,node2,edge_directed));
	edges_by_nodes[node1->id][node2->id] = new_edge_v1_v2;
	neighbors_out[node1->id].insert(node2->id,node2);
	neighbors_in[node2->id].insert(node1->id,node1);
	neighbors_all[node1->id].insert(node2->id,node2);
	neighbors_all[node2->id].insert(node1->id,node1); // if node1 and node2 are the same, it also works
	edges_by_layers_and_id[node1->layer->id][node2->layer->id].insert(eid,new_edge_v1_v2);
	edges_by_id.insert(eid,new_edge_v1_v2);
	if (!edge_directed) {
		// create an additional edge with the same identifier but inverted nodes
		EdgeSharedPtr new_edge_v2_v1(new edge(eid,node2,node1,edge_directed));
		neighbors_out[node2->id].insert(node1->id,node1);
		neighbors_in[node1->id].insert(node2->id,node2);
		if (node1->layer->id!=node2->layer->id)
			edges_by_layers_and_id[node2->layer->id][node1->layer->id].insert(eid,new_edge_v2_v1);
	}
	return new_edge_v1_v2;
}

EdgeSharedPtr MLNetwork::get_edge(const NodeSharedPtr& node1, const NodeSharedPtr& node2) const {
	if (edges_by_nodes.count(node1->id)==0 ||
		edges_by_nodes.at(node1->id).count(node2->id)==0) return NULL;
	else return edges_by_nodes.at(node1->id).at(node2->id);
}

const ObjectStore<EdgeSharedPtr>& MLNetwork::get_edges() const {
	return edges_by_id;
}

const ObjectStore<EdgeSharedPtr>& MLNetwork::get_edges(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) const {
	if (edges_by_layers_and_id.count(layer1->id)==0 || edges_by_layers_and_id.at(layer1->id).count(layer2->id)==0)
		return *(new ObjectStore<EdgeSharedPtr>());
	return edges_by_layers_and_id.at(layer1->id).at(layer2->id);
}

void MLNetwork::erase(const NodeSharedPtr& node) {
	// removing adjacent edges
	ObjectStore<NodeSharedPtr> in = neighbors(node,IN);
	for (NodeSharedPtr node_in : in) {
		EdgeSharedPtr edge = get_edge(node_in,node);
		erase(edge);
	}
	ObjectStore<NodeSharedPtr> out = neighbors(node,OUT);
	for (NodeSharedPtr node_out : out) {
			EdgeSharedPtr edge = get_edge(node,node_out);
		erase(edge);
	}

	// removing the node
	nodes_by_id.erase(node->id);
	nodes_by_layer_and_id[node->layer->id].erase(node->id);
	nodes_by_layer_and_name[node->layer->id].erase(node->name);

	// remove attribute values
	node_features(node->layer)->remove(node->id);
}

void MLNetwork::erase(const EdgeSharedPtr& edge) {
	edges_by_id.erase(edge->id);
	edges_by_layers_and_id[edge->v1->layer->id][edge->v2->layer->id].erase(edge->id);
	edges_by_nodes[edge->v1->id].erase(edge->v2->id);
	neighbors_in[edge->v2->id].erase(edge->v1->id);
	neighbors_out[edge->v1->id].erase(edge->v2->id);
	neighbors_all[edge->v2->id].erase(edge->v1->id);
	neighbors_all[edge->v1->id].erase(edge->v2->id);
	if (!edge->directed) {
		if (edge->v1->layer->id!=edge->v2->layer->id)
			edges_by_layers_and_id[edge->v2->layer->id][edge->v1->layer->id].erase(edge->id);
		edges_by_nodes[edge->v2->id].erase(edge->v1->id);
		neighbors_in[edge->v1->id].erase(edge->v2->id);
		neighbors_out[edge->v2->id].erase(edge->v1->id);
		neighbors_all[edge->v1->id].erase(edge->v2->id);
		neighbors_all[edge->v2->id].erase(edge->v1->id);
	}

	// remove attribute values
	edge_features(edge->v1->layer,edge->v2->layer)->remove(edge->id);
}

/*****************************************************************************************
 * Functions returning information about the MLNetwork.
 *****************************************************************************************/

const ObjectStore<NodeSharedPtr>& MLNetwork::neighbors(const NodeSharedPtr& node, int mode) const {
	if (mode==IN)
		return neighbors_in.at(node->id);
	else if (mode==OUT)
		return neighbors_out.at(node->id);
	else if (mode==INOUT)
		return neighbors_all.at(node->id);
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
	return "Multilayer Network (\"" + name() + "\": " + std::to_string(get_layers().size()) + " layers, " +
			std::to_string(get_actors().size()) + " actors, " +
			std::to_string(get_nodes().size()) + " nodes, " +
			std::to_string(get_edges().size()) + " edges)";
}

} // namespace mlnet
