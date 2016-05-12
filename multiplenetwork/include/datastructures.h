/**
 * datastructures.h
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 1.0
 *
 * This file defines the basic data structure of the library: the MLNetwork class - where
 * "ML" stands for "multilayer" and not for "Matteo & Luca", as in the original paper
 * "The ML-model for multi-layer social networks" :). Other related data types are also defined:
 * 1. The basic components of a multilayer network (layer, node, edge, actor, and their
 *    abstract super-types, basic_component and named_component).
 *    These components are encapsulated inside a MLNetwork and programmers using this library do
 *    not need to deal with them: they are created, retrieved and updated by MLNetwork objects.
 * 2. An AttributeStore class, to associate attributes to different components
 *    (actors, nodes...) in a multilayer network, together with a class Attribute. This class
 *    encapsulates attribute management outside MLNetworks, so that it can be substituted with
 *    other storage systems if needed, like a relational database.
 * 2. Classes to represent paths and distances crossing multiple layers.
 * 3. Short names and constants used throughout the package:
 *    ActorSharedPtr, NodeSharedPtr, ..., ml_matrix, ml_hashtable, ..., edge_mode, attribute_type, ...
 */

#ifndef MLNET_DATASTRUCTURES_H_
#define MLNET_DATASTRUCTURES_H_

#include "utils.h"
#include "exceptions.h"
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <memory>

namespace mlnet {


/**********************************************************************/
/** Main classes defined in this file *********************************/
/** (full definitions follow) *****************************************/
/**********************************************************************/

/* Identifiers */
typedef long object_id; // for generic components
typedef object_id actor_id;
typedef object_id layer_id;
typedef object_id node_id;
typedef object_id edge_id;

/* Classes */
class actor;
class layer;
class node;
class edge;
class clique;
class MLNetwork;
class AttributeStore;
class Attribute;
class path;

/**********************************************************************/
/** Shorthands ********************************************************/
/**********************************************************************/

/* smart pointers */
typedef std::shared_ptr<MLNetwork> MLNetworkSharedPtr;
typedef std::shared_ptr<const MLNetwork> constMLNetworkSharedPtr;
typedef std::shared_ptr<edge> EdgeSharedPtr;
typedef std::shared_ptr<actor> ActorSharedPtr;
typedef std::shared_ptr<layer> LayerSharedPtr;
typedef std::shared_ptr<node> NodeSharedPtr;
typedef std::shared_ptr<Attribute> AttributeSharedPtr;
typedef std::shared_ptr<AttributeStore> AttributeStoreSharedPtr;

/* Lists of components */
typedef sorted_random_map<actor_id, ActorSharedPtr> actor_list;
typedef sorted_random_map<layer_id, LayerSharedPtr> layer_list;
typedef sorted_random_map<node_id, NodeSharedPtr> node_list;
typedef sorted_random_map<edge_id, EdgeSharedPtr> edge_list;

/* Other shorthands, for tidiness */
typedef std::string string;

/**********************************************************************/
/** Constants and Function Parameters *********************************/
/**********************************************************************/

/** Directionality of edges, when not specified */
const bool DEFAULT_EDGE_DIRECTIONALITY = false; // edges are undirected by default

/** Default name for weight attribute */
const string DEFAULT_WEIGHT_ATTR_NAME = "ML_WEIGHT";

/** Selection mode, for directed edges (e.g., to compute the IN-degree or OUT-degree of a node) */
enum edge_mode {INOUT=0, IN=1, OUT=2};

/** Supported attribute types in the current implementation of AttributeStore */
enum attribute_type {STRING_TYPE = 0, NUMERIC_TYPE = 1};

/**
 * The length of a path traversing multiple layers can be represented
 * using a matrix m, where cell m_{i,j} indicates the number
 * of edges traversed from layer i to layer j.
 * These constants specify how much of this information should be used
 * while comparing two path lengths:
 * - FULL: all the elements in the matrix are considered.
 * - SWITCH_COSTS: the diagonal elements are considered (indicating inter-layer steps),
 *   plus an additional element summing the values on all non-diagonal cells (indicating a layer crossing).
 * - MULTIPLEX: only the diagonal is considered (that is, only inter-layer steps).
 * - SIMPLE: the sum of all elements in the matrix (that is, the number of steps
 *   irrespective of the traversed layers) is considered.
 **/
enum comparison_type {FULL = 0, SWITCH_COSTS = 1, MULTIPLEX = 2, SIMPLE = 3};

/** Outcome of the comparison between two numbers, or two vectors, or two matrices. (the concept of Pareto dominance is used). */
enum comparison_result {LESS_THAN=0, EQUAL=1, INCOMPARABLE=2, GREATER_THAN=3};


/**********************************************************************/
/** MLNetwork components and basic motifs *****************************/
/**********************************************************************/

/**
 * A generic basic component in a MLNetwork.
 */
class basic_component {
protected:
	/** Constructor */
	basic_component(object_id id);
	/** Output function, presenting a complete description of the component */
	string to_string() const;
public:
	/** Unique identifier of the component */
	const object_id id;
	/** Comparison operator: equality, based on the object identifiers. This assumes that objects of the same type are compared - no checks are made for efficiency reasons. */
	bool operator==(const basic_component& comp) const;
	/** Comparison operator: difference, based on the object identifiers. This assumes that objects of the same type are compared - no checks are made for efficiency reasons. */
	bool operator!=(const basic_component& comp) const;
	/** Comparison operator: less than, based on the object identifiers. This assumes that objects of the same type are compared - no checks are made for efficiency reasons. */
	bool operator<(const basic_component& comp) const;
	/** Comparison operator: higher than, based on the object identifiers. This assumes that objects of the same type are compared - no checks are made for efficiency reasons. */
	bool operator>(const basic_component& comp) const;
};

/**
 * A basic component of a MLNetwork, which can also be identified by name.
 */
class named_component : public basic_component {
protected:
	/** Constructor */
	named_component(object_id id, const string& name);
	/** Output function, presenting a complete description of the component */
	string to_string() const;
	friend std::ostream& operator<<(std::ostream& os, const named_component& dt);
public:
	/** Unique name of the component */
	const string name;
};

/**
 * An actor in a MLNetwork.
 */
class actor : public named_component {
public:
	/** Constructor */
	actor(actor_id id, const string& name);
	/** Output function, presenting a complete description of the actor */
	string to_string() const;
};

/**
 * A layer in a MLNetwork.
 */
class layer : public named_component {
public:
	/** Constructor */
	layer(layer_id id, const string& name);
	/** Output function, presenting a complete description of the layer */
	string to_string() const;
};


/**
 * A node inside a MLNetwork.
 */
class node : public basic_component {
public:
	/** The actor corresponding to this node */
	const ActorSharedPtr actor;
	/** The layer where this node is located */
	const LayerSharedPtr layer;
	/** Constructor */
	node(node_id id, const ActorSharedPtr& actor, const LayerSharedPtr& layer);
	/** Output function, presenting a complete description of the node */
	string to_string() const;
};


/**
 * An edge between two nodes in a MLNetwork.
 */
class edge : public basic_component  {
public:
	/** The node at the first end of this edge */
	const NodeSharedPtr v1;
	/** The node at the second end of this edge */
	const NodeSharedPtr v2;
	/** Edge directionality */
	const bool directed;
	/** Constructor */
	edge(edge_id id, const NodeSharedPtr& v1, const NodeSharedPtr& v2, bool directed);
	/** Output function, presenting a complete description of the edge */
	string to_string() const;
};

class actor_set {
public:
	/** Constructors */
	actor_set(const simple_set<ActorSharedPtr>& actors);
	actor_set();
	/** Comparison operator: equality, based on the presence of the same actors and the same layers. */
	bool operator==(const actor_set& comp) const;
	/** Comparison operator: difference, based on the presence of the same actors and the same layers. */
	bool operator!=(const actor_set& comp) const;
	/** Comparison operator: less then, based on the < operator of the actors and (if needed) the layers in the clique. */
	bool operator<(const actor_set& comp) const;
	/** Comparison operator: greater than, based on the > operator of the actors and (if needed) the layers in the clique. */
	bool operator>(const actor_set& comp) const;
	/** Output function, presenting a complete description of the clique */
	std::string to_string();

	sorted_set<ActorSharedPtr> actors;
};

class dyad : public actor_set {
public:
	/** Constructors */
	dyad(const ActorSharedPtr& actor1, const ActorSharedPtr& actor2);
};

class triad : public actor_set {
public:
	/** Constructors */
	triad(const ActorSharedPtr& actor1, const ActorSharedPtr& actor2, const ActorSharedPtr& actor3);
};

typedef std::shared_ptr<dyad> DyadSharedPtr;
typedef std::shared_ptr<triad> TriadSharedPtr;
typedef std::shared_ptr<actor_set> ActorSetSharedPtr;

/**
 * A multilayer clique.
 */
class clique {
public:
	/** Constructors */
	clique(const simple_set<ActorSharedPtr>& actors, const simple_set<LayerSharedPtr>& layers);
	clique();
	/** Comparison operator: equality, based on the presence of the same actors and the same layers. */
	bool operator==(const clique& comp) const;
	/** Comparison operator: difference, based on the presence of the same actors and the same layers. */
	bool operator!=(const clique& comp) const;
	/** Comparison operator: less then, based on the < operator of the actors and (if needed) the layers in the clique. */
	bool operator<(const clique& comp) const;
	/** Comparison operator: greater than, based on the > operator of the actors and (if needed) the layers in the clique. */
	bool operator>(const clique& comp) const;
	/** Output function, presenting a complete description of the clique */
	std::string to_string();

	sorted_set<ActorSharedPtr> actors;
	sorted_set<LayerSharedPtr> layers;
};

typedef std::shared_ptr<clique> CliqueSharedPtr;

/**********************************************************************/
/** Attribute handling ************************************************/
/**********************************************************************/

/**
 * Meta data about an attribute in an AttributeStore
 */
class Attribute {
public:
	/**
	 * Creates a new Attribute.
	 * @param name name of the attribute
	 * @param type type of the attribute (see attribute_type enumeration: STRING_TYPE, NUMERIC_TYPE)
	 */
	Attribute(const string& name, attribute_type type);

	/**
	 * @brief Returns the name of the attribute
	 * @return the name of the attribute.
	 **/
	const string& name() const;

	/**
	 * @brief Returns the type of the attribute.
	 * @return the type of the attribute.
	 **/
	attribute_type type() const;

	/**
	 * @brief Returns a string representation of the type of the attribute.
	 * @return a string representation of the type of the attribute.
	 **/
	string type_as_string() const;

private:
	const string aname;
	const attribute_type atype;
};

/**
 * A class associating multiple attributes and attribute values to a set of objects.
 *
 * This class does not check if objects exist and does not require objects
 * to be explicitly registered into it. Whenever an object that has not been
 * explicitly registered is queried, default attribute values are returned.
 * Therefore, checks about the existence of an object should be performed at
 * the MLNetwork level.
 */
class AttributeStore {
public:
	/**
	* Creates an empty AttributeStore and returns a pointer to it. Attribute stores
	* are only created using this method throughout the library, so that the type of
	* attribute store can be easily changed. For now, only a main memory attribute store
	* is available.
	* @return a pointer to the new attribute store
	*/
	static AttributeStoreSharedPtr create();

	/** virtual destructor */
	virtual ~AttributeStore();

	/**
	 * @brief Returns the number of attributes in this store, of all types.
	 * @return the number of attributes in this store
	 **/
	virtual int numAttributes() const = 0;

	/**
	 * @brief Returns the attributes in this store.
	 * @return a vector containing all the attributes.
	 **/
	virtual const vector<AttributeSharedPtr>& attributes() const = 0;

	/**
	 * @brief Returns the n^th attribute in this store.
	 * @param idx the position of the attribute (from 0 to numAttributes()-1).
	 * @return an object of type Attribute, or NULL if idx's attribute does not exist.
	 **/
	virtual AttributeSharedPtr attribute(int idx) const = 0;

	/**
	 * @brief Returns an attribute by name.
	 * @param name The name of the queried attribute.
	 * @return an object of type AttributeSharedPtr, or NULL if an attribute with this name does not exist.
	 **/
	virtual AttributeSharedPtr attribute(const string& name) const = 0;

	/**
	 * @brief Enables the association of a value to each object in this store.
	 * @param attribute_name The name of the attribute
	 * @param type The type of the attribute
	 * - STRING_TYPE: c++ "string" type
	 * - NUMERIC_TYPE: c++ "double" type
	 * @throws DuplicateElementException if an attribute with this name already exists
	 **/
	virtual void add(const string& attribute_name, attribute_type type) = 0;

	/**
	 * @brief Sets the value of an attribute.
	 * @param id the id of the object whose associated value is set
	 * @param attribute_name The name of the attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no attribute with this name
	 * @throws OperationNotSupportedException if the attribute type is not STRING_TYPE
	 **/
	virtual void setString(object_id id, const string& attribute_name, const string& value) = 0;

	/**
	 * @brief Sets the value of an attribute.
	 * @param id the id of the object whose associated value is set
	 * @param attribute_name The name of the attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no attribute with this name
	 * @throws OperationNotSupportedException if the attribute type is not NUMERIC_TYPE
	 **/
	virtual void setNumeric(object_id id, const string& attribute_name, double value) = 0;

	/**
	 * @brief Gets the value of an attribute.
	 * @param id the id of the object whose associated value is retrieved
	 * @param attribute_name The name of the attribute
	 * @return The value associated to the object, or null if the object id has not been registered in this store
	 * @throws ElementNotFoundException if there is no attribute with this name
	 **/
	virtual const string& getString(object_id id, const string& attribute_name) const = 0;

	/**
	 * @brief Gets the value of an attribute.
	 * @param id the id of the object whose associated value is retrieved
	 * @param attribute_name The name of the attribute
	 * @throws ElementNotFoundException if there is no object with this id
	 **/
	virtual const double& getNumeric(object_id id, const string& attribute_name) const = 0;

	/**
	 * @brief Removes all the attribute values from an object.
	 * If the same object is queried after this method has been called, default values will be returned.
	 * @param id The id of the object to be removed from the store.
	 **/
	virtual void reset(object_id id) = 0;

protected:
	/** default value for numeric attributes */
	double default_numeric = 0.0;
	/** default value for string attributes */
	string default_string = "";
};

class MainMemoryAttributeStore: public AttributeStore {
public:
	MainMemoryAttributeStore();

	// All the following methods are documented in the super-class AttributeStore

	int numAttributes() const;
	const vector<AttributeSharedPtr>& attributes() const;
	AttributeSharedPtr attribute(int idx) const;
	AttributeSharedPtr attribute(const string& name) const;
	void add(const string& attribute_name, attribute_type type);
	void setString(object_id id, const string& attribute_name, const string& value);
	void setNumeric(object_id id, const string& attribute_name, double value);
	const string& getString(object_id id, const string& attribute_name) const;
	const double& getNumeric(object_id id, const string& attribute_name) const;
	void reset(object_id id);

private:
	/* meta-data: names and types of attributes */
	vector<AttributeSharedPtr> attribute_vector;
	hashtable<string,int> attribute_ids;
	/* These hash maps are structured as: map[AttributeName][object_id][AttributeValue] */
	hashtable<string, hashtable<object_id, string> > string_attribute;
	hashtable<string, hashtable<object_id, double> > numeric_attribute;
};

/**********************************************************************/
/** MLNetwork *********************************************************/
/**********************************************************************/

/**
 * Main data structure of the package, defining a multilayer network.
 */
class MLNetwork {

	/**************************************************************************************
	 * Constructors/destructors
	 **************************************************************************************/
private:
	/**
	 * Creates an empty MLNetwork.
	 * This is only used internally: to create a new network use MLNetwork::create() instead (or
	 * any IO function available in another module of the library).
	 * @param name name of the new MLNetwork
	 */
	MLNetwork(const string& name);

public:

	/** Name of the multilayer network */
	const string name;

	/**
	 * Creates an empty MLNetwork and returns a pointer to it.
	 * @param name name of the new multilayer network
	 * @return a pointer to the new multilayer network
	 */
	static MLNetworkSharedPtr create(const string& name);

	/**************************************************************************************
	 * Basic structural operations used to modify/access MLNetwork components
	 **************************************************************************************/

	/**
	 * @brief Adds a new actor to the MLNetwork.
	 * A new identifier is automatically associated to the new actor.
	 * @param name name of the actor
	 * @return a pointer to the new actor, or a NULL pointer if an actor with the same name exists.
	 **/
	ActorSharedPtr add_actor(const string& name);

	/**
	 * @brief Returns an actor by ID.
	 * This method can also be used to check if an actor is present in the MLNetwork.
	 * @param id identifier of the actor
	 * @return a pointer to the requested actor, or null if the actor does not exist
	 **/
	ActorSharedPtr get_actor(actor_id id) const;

	/**
	 * @brief Returns an actor by name.
	 * This function can also be used to check if an actor is present in the MLNetwork.
	 * @param name name of the actor
	 * @return a pointer to the requested actor, or null if the actor does not exist
	 **/
	ActorSharedPtr get_actor(const string& name) const;

	/**
	 * @brief Returns all the actors in the MLNetwork.
	 * @return a pointer to an actor iterator
	 **/
	const actor_list& get_actors() const;

	/**
	 * @brief Adds a new layer to the MLNetwork.
	 * A new identifier is automatically associated to the new layer. If the layer is
	 * already present in the network, a NULL pointer is returned indicating that
	 * the operation failed.
	 * @param name name of the layer
	 * @param directed TRUE or FALSE
	 * @return a pointer to the new layer, or a NULL pointer if a layer with the same name exists.
	 **/
	LayerSharedPtr add_layer(const string& name, bool directed);

	/**
	 * @brief Returns a layer by ID.
	 * This function can also be used to check if a layer is present in the MLNetwork
	 * @param id identifier of the layer
	 * @return a pointer to the requested layer, or null if the layer does not exist
	 **/
	LayerSharedPtr get_layer(layer_id id) const;

	/**
	 * @brief Returns a layer by name.
	 * This function can also be used to check if a layer is present in the MLNetwork
	 * @param name name of the layer
	 * @return a pointer to the requested layer, or null if the layer does not exist
	 **/
	LayerSharedPtr get_layer(const string& name) const;

	/**
	 * @brief Returns all the layers in the MLNetwork.
	 * @return a layer iterator
	 **/
	const layer_list& get_layers() const;

	/**
	 * @brief Sets the default edge directionality depending on the layers of the connected nodes.
	 * The directionality of (layer1,layer2) is the same as (layer2,layer1) - that is, if edges are directed
	 * in one direction, they must be directed also in the other.
	 * @param layer1 a pointer to the "from" layer
	 * @param layer2 a pointer to the "to" layer
	 * @param directed TRUE or FALSE
	 * @return a pointer to the new layer
	 **/
	void set_directed(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2, bool directed);

	/**
	 * @brief Gets the default edge directionality depending on the layers of the connected nodes.
	 * @return a Boolean value indicating if edges among these two layers are directed or not.
	 **/
	bool is_directed(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) const;

	/**
	 * @brief Adds a new node to the MLNetwork.
	 * A new identifier is automatically associated to the new node
	 * @param layer pointer to the layer where this node is located
	 * @param actor pointer to the actor corresponding to this node
	 * @return a pointer to the new node, or a NULL pointer if the actor is already present in the layer.
	 * @throws ElementNotFoundException if the input layer or actor are not present in the network
	 **/
	NodeSharedPtr add_node(const ActorSharedPtr& actor, const LayerSharedPtr& layer);

	/**
	 * @brief Returns a node by ID.
	 * This function can also be used to check if a node is present in the MLNetwork
	 * @param id identifier of the layer
	 * @return a pointer to the requested node, or null if the node does not exist
	 **/
	NodeSharedPtr get_node(node_id id) const;

	/**
	 * @brief Returns a node by specifying an actor and a layer.
	 * This function can also be used to check if a node is present in the MLNetwork
	 * @param actor the actor to which the searched node corresponds
	 * @param layer the layer where the searched node is located
	 * @return a pointer to the requested node, or null if the node does not exist
	 **/
	NodeSharedPtr get_node(const ActorSharedPtr& actor, const LayerSharedPtr& layer) const;

	/**
	 * @brief Returns all the nodes in the MLNetwork.
	 * @return a node iterator
	 **/
	const node_list& get_nodes() const;

	/**
	 * @brief Returns all the nodes in a layer.
	 * @param layer pointer to the layer where this node is located
	 * @return a node iterator
	 **/
	const node_list& get_nodes(const LayerSharedPtr& layer) const;

	/**
	 * @brief Returns the nodes associated to the input actor.
	 * @param actor pointer to the actor
	 * @return an iterator containing pointers to nodes
	 **/
	const node_list& get_nodes(const ActorSharedPtr& actor) const;

	/**
	 * @brief Adds a new edge to the MLNetwork.
	 * Multiple edges between the same pair of nodes are not allowed. The directionality of the
	 * edge is defined by the layers of the two nodes.
	 * @param node1 a pointer to the "from" node if directed, or to one end of the edge if undirected
	 * @param node2 a pointer to the "to" node if directed, or one end of the edge if undirected
	 * @return a pointer to the new edge, or a NULL pointer if the edge already exists.
	 * @throws ElementNotFoundException if the input nodes are not present in the network
	 **/
	EdgeSharedPtr add_edge(const NodeSharedPtr& node1, const NodeSharedPtr& node2);

	/**
	 * @brief Returns an edge.
	 * This function can also be used to check if an edge is present in the MLNetwork
	 * @param node1 a pointer to the "from" node if directed, or to one end of the edge if undirected
	 * @param node2 a pointer to the "to" node if directed, or one end of the edge if undirected
	 * @return a pointer to the requested edge, or null if the edge does not exist
	 **/
	EdgeSharedPtr get_edge(const NodeSharedPtr& node1,  const NodeSharedPtr& node2) const;

	/**
	 * @brief Returns all the edges in the MLNetwork.
	 * @return an edge iterator
	 **/
	const edge_list& get_edges() const;

	/**
	 * @brief Returns all the edges from a layer A to a layer B.
	 * This method can also retrieve intra-layer edges setting A = B.
	 * @param layer1 pointer to the layer where the first ends of the edges are located
	 * @param layer2 pointer to the layer where the second ends of the edges are located
	 * @return an edge iterator
	 **/
	const edge_list& get_edges(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) const;

	/**
	 * @brief Deletes an existing node.
	 * All related data, including node attributes and edges involving this node, are also deleted.
	 * @param node a pointer to the node to be deleted
	 * @return true if the object has been removed, false if it was not present in the network.
	 **/
	bool erase(const NodeSharedPtr& node);

	/**
	 * @brief Deletes an existing edge.
	 * Attribute values associated to this edge are also deleted.
	 * @param edge a pointer to the edge to be deleted
	 * @return true if the object has been removed, false if it was not present in the network.
	 **/
	bool erase(const EdgeSharedPtr& edge);

	/**
	 * @brief Deletes an existing actor.
	 * All related data, including node attributes and edges involving this actor, are also deleted.
	 * @param actor a pointer to the actor to be deleted
	 * @return true if the object has been removed, false if it was not present in the network.
	 **/
	bool erase(const ActorSharedPtr& actor);

	/**
	 * @brief Deletes an existing layer.
	 * Attribute values associated to this edge are also deleted.
	 * @param layer a pointer to the layer to be deleted
	 * @return true if the object has been removed, false if it was not present in the network.
	 **/
	bool erase(const LayerSharedPtr& layer);

	/**
	 * @brief Returns the nodes with an edge from/to the input node.
	 * @param node pointer to the node
	 * @param mode IN, OUT or INOUT
	 * @return an iterator containing pointers to nodes
	 * @throws WrongParameterException if mode is not one of IN, OUT or INOUT
	 **/
	const node_list& neighbors(const NodeSharedPtr& node, edge_mode mode) const;

	/******************************
	 * Attribute handling
	 ******************************/

	/**
	 * @brief Allows the manipulation of feature vectors associated to actors.
	 * This function is not thread-safe.
	 * @return an AttributeStore storing actor features
	 **/
	AttributeStoreSharedPtr actor_features();
	/**
	 * @brief Allows the inspection of feature vectors associated to actors.
	 * This function is not thread-safe.
	 * @return a constant AttributeStore storing actor features
	 **/
	const AttributeStoreSharedPtr actor_features() const;

	/**
	 * @brief Allows the manipulation of feature vectors associated to layers.
	 * This function is not thread-safe.
	 * @return an AttributeStore storing layer features
	 **/
	AttributeStoreSharedPtr layer_features();
	/**
	 * @brief Allows the inspection of feature vectors associated to layers.
	 * This function is not thread-safe.
	 * @return a constant AttributeStore storing layer features
	 **/
	const AttributeStoreSharedPtr layer_features() const;

	/**
	 * @brief Allows the manipulation of feature vectors associated to nodes.
	 * Every layer can associate different features to its nodes, but all nodes in the same layer have the same features
	 * This function is not thread-safe.
	 * @param layer pointer to the layer where the nodes are located
	 * @return an AttributeStore storing node features
	 **/
	AttributeStoreSharedPtr node_features(const LayerSharedPtr& layer);
	/**
	 * @brief Allows the manipulation of feature vectors associated to nodes.
	 * Every layer can associate different features to its nodes, but all nodes in the same layer have the same features
	 * This function is not thread-safe.
	 * @param layer pointer to the layer where the nodes are located
	 * @return a constant AttributeStore storing node features
	 **/
	const AttributeStoreSharedPtr node_features(const LayerSharedPtr& layer) const;

	/**
	 * @brief Allows the manipulation of feature vectors associated to edges.
	 * Every pair of layers corresponds to different edge features, but all edges between the same pair of layers have the same features
	 * This function is not thread-safe.
	 * @param layer1 pointer to the layer where the first ends of the edges are located
	 * @param layer2 pointer to the layer where the second ends of the edges are located
	 * @return an AttributeStore storing edge features
	 **/
	AttributeStoreSharedPtr edge_features(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);
	/**
	 * @brief Allows the inspection of feature vectors associated to edges.
	 * Every pair of layers corresponds to different edge features, but all edges between the same pair of layers have the same features
	 * This function is not thread-safe.
	 * @param layer1 pointer to the layer where the first ends of the edges are located
	 * @param layer2 pointer to the layer where the second ends of the edges are located
	 * @return a constant AttributeStore storing edge features
	 **/
	const AttributeStoreSharedPtr edge_features(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) const;

	/**
	 * @brief Commodity function to set the weight of an edge.
	 * The weight attribute (DEFAULT_WEIGHT_ATTR_NAME) must exist.
	 * @param node1 a pointer to the "from" node if directed, or to one end of the edge if undirected
	 * @param node2 a pointer to the "to" node if directed, or one end of the edge if undirected
	 * @param weight the weight to be set
	 * @throw ElementNotFoundException if the edge does not exist
	 **/
	void set_weight(const NodeSharedPtr& node1, const NodeSharedPtr& node2, double weight);

	/**
	 * @brief Commodity function to get the weight of an edge.
	 * The weight attribute (DEFAULT_WEIGHT_ATTR_NAME) must exist.
	 * @param node1 a pointer to the "from" node if directed, or to one end of the edge if undirected
	 * @param node2 a pointer to the "to" node if directed, or one end of the edge if undirected
	 * @throw ElementNotFoundException if the edge does not exist
	 * @return the weight on the edge
	 **/
	double get_weight(const NodeSharedPtr& node1, const NodeSharedPtr& node2);

	/** Returns a compact string representation of this MLNetwork */
	string to_string() const;

private:
	// largest identifier assigned so far
	node_id max_node_id;
	edge_id max_edge_id;
	actor_id max_actor_id;
	layer_id max_layer_id;

	/* Edge directionality */
	hashtable<layer_id, hashtable<layer_id, bool> > edge_directionality;

	// Components:
	layer_list layers;
	actor_list actors;
	node_list nodes;
	edge_list edges;

	// Indexes to components (Component IDX):
	hashtable<string, LayerSharedPtr> cidx_layer_by_name;
	hashtable<string, ActorSharedPtr> cidx_actor_by_name;
	hashtable<actor_id, hashtable<layer_id, NodeSharedPtr > > cidx_node_by_actor_and_layer;
	hashtable<node_id, hashtable<node_id, EdgeSharedPtr> > cidx_edge_by_nodes;

	// Indexes to sets of components (Set IDX):
	hashtable<layer_id, node_list> sidx_nodes_by_layer;
	hashtable<actor_id, node_list> sidx_nodes_by_actor;
	hashtable<layer_id, hashtable<layer_id, edge_list> > sidx_edges_by_layer_pair;

	hashtable<node_id, node_list> sidx_neighbors_out;
	hashtable<node_id, node_list> sidx_neighbors_in;
	hashtable<node_id, node_list> sidx_neighbors_all;

	/* objects storing the feature vectors of the different components */
	AttributeStoreSharedPtr actor_attributes;
	AttributeStoreSharedPtr layer_attributes;
	hashtable<layer_id, AttributeStoreSharedPtr> node_attributes;
	hashtable<layer_id, hashtable<layer_id, AttributeStoreSharedPtr> > edge_attributes;

	/* An empty set of nodes, conveniently returned when necessary instead of creating a new empty node_list. */
	node_list empty;
};


/**********************************************************************/
/** Paths *************************************************************/
/**********************************************************************/

/**
 * This class represents the length of a path in a multilayer network.
 * It is represented using a matrix where element {i,j} indicates the number
 * of edges traversed from layer i to layer j.
 */
class path_length {
private:
	/** The multilayer network to which this distance refers. */
	MLNetworkSharedPtr mnet;
	/** Number of steps for each pair of layers. (This includes intra-layer steps). */
	PairCounter<layer_id,layer_id> num_edges;
	/** Total number of steps, irrespective of the layers */
	long total_length;

public:
	long ts;

	/** Constructs an empty distance. */
	path_length(const MLNetworkSharedPtr& mnet);

	/**
	 * Increases this distance by a new step from a node to another.
	 * @param layer1 the starting layer of the new step.
	 * @param layer2 the arrival layer of the new step.
	 */
	void step(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2);

	/**
	 * @return The total number of steps (that is, traversed edges).
	 */
	long length() const;

	/**
	 * @return The number of steps (that is, traversed edges) inside a given layer.
	 * @param layer only edges between nodes in this layer are considered.
	 */
	long length(const LayerSharedPtr& layer) const;

	/**
	 * @return The number of steps (that is, traversed edges) from a node in layer from to a node in layer to.
	 * @param from first layer.
	 * @param to second layer.
	 */
	long length(const LayerSharedPtr& from, const LayerSharedPtr& to) const;

	/**
	 * @brief Compares two distances according to the comp parameter:
	 * @param other The distance to be compared to.
	 * @param comp This parameter specifies the amount of information used while comparing the two distances.
	 * For the allowed values, see the definition of mlnet::comparison_type.
	 * @return One relationship of type mlnet::comparison_result
	 */
	comparison_result compare(const path_length& other, comparison_type comp) const;

	/**
	 * Comparison by id. For a comparison considering steps
	 * on different layers as incomparable entities, use compare()
	 * @param other The distance to be compared to.
	 * @return true if this distance's id is shorter than the input one.
	 */
	bool operator<(const path_length& other) const;

	/**
	 * Comparison by id. For a comparison considering steps
	 * on different layers as incomparable entities, use compare()
	 * @param other The distance to be compared to.
	 * @return true if this distance is longer than the input one.
	 */
	bool operator>(const path_length& other) const;

	/**
	 * Comparison by id. For a comparison considering steps
	 * on different layers as incomparable entities, use compare()
	 * @param other The distance to be compared to.
	 * @return true if this distance is the same as the input one.
	 */
	bool operator==(const path_length& other) const;

	/**
	 * Compare the absolute length of the two distances. For a comparison considering steps
	 * on different layers as incomparable entities, use compare()
	 * @param other The distance to be compared to.
	 * @return true if this distance is different from the input one.
	 */
	bool operator!=(const path_length& other) const;

	/** Returns a string representation of this object */
	string to_string() const;

private:

	comparison_result compare_full(const path_length& other) const;

	comparison_result compare_switch(const path_length& other) const;

	comparison_result compare_multiplex(const path_length& other) const;

	comparison_result compare_simple(const path_length& other) const;
};

} // namespace mlnet

// Assumes a limited number of elements in the set TODO test collisions
namespace std {
template <>
struct hash<mlnet::dyad> {
	size_t operator()(const mlnet::dyad& a) const {
		size_t h = ~0;
		int shift = 5;
		for (mlnet::ActorSharedPtr actor: a.actors) {
			h += h ^ (actor->id << shift);
			shift += 5;
		}
		return h;
    }
};
}


#endif /* MLNET_DATASTRUCTURES_H_ */
