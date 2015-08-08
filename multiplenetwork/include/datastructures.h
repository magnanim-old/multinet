/**
 * datastructures.h
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 1.0
 *
 * This file defines the basic data structures of the library. It includes:
 * 
 * 1. MLNetwork (where ML stands for multilayer), the main class defined in this file.
 * 2. Basic components of a MLNetwork (layer, node, edge, actor).
 *    An actor represents a global identity, and multiple nodes
 *    (organized into multiple layers) can correspond to the same actor.
 *    An example is an individual (actor) with multiple accounts (nodes)
 *    on different online social networks (layers).
 * 3. An ObjectSet class, to store a set of pointers to basic network components.
 *    This class is optimized so that:
 *    (a) Functions potentially returning many pointers to entities
 *    (e.g., "return all neighbors of a node") do not
 *    directly return all the pointers, but only an object that allows
 *    to iterate over them.
 *    (b) It allows efficient (log n) selection both by key and by position, which
 *    is useful e.g. to randomly select objects in the ML network.
 * 4. An AttributeStore class, to associate attributes to different objects
 *    (actors, nodes...) in the MLNetwork, together with a class Attribute to represent
 *    metadata.
 * 5. Smart pointers to all these objects.
 *    Only one instance of each entity (e.g., node) is kept in memory,
 *    and can be accessed through multiple indexes (e.g., by ID, by name)
 *    storing pointers to the entities. Functions accessing the MLNetwork's
 *    components return pointers as well, so that the objects are never
 *    duplicated after their creation.
 *
 * All the definitions are in the "mlnet" namespace
 */

#ifndef MLNET_DATASTRUCTURES_H_
#define MLNET_DATASTRUCTURES_H_

#include <string>
#include <map>
#include "exceptions.h"
#include "random.h"
#include <cmath>
#include <set>
#include <vector>
#include <memory>
#include <cstdlib>

namespace mlnet {

class MLNetwork;
/** A smart pointer to objects of type MLNetwork */
typedef std::shared_ptr<MLNetwork> MLNetworkSharedPtr;
/** A smart pointer to constant objects of type MLNetwork */
typedef std::shared_ptr<const MLNetwork> constMLNetworkSharedPtr;

/**********************************************************************/
/** Constants and Function Parameters *********************************/
/**********************************************************************/

const bool DEFAULT_EDGE_DIRECTIONALITY = false; // undirected edges by default

/** Selection mode, for directed edges (e.g., to compute the IN-degree or OUT-degree of a node) */
enum edge_mode {INOUT=0, IN=1, OUT=2};

/** Supported attribute types */
enum attribute_type {STRING_TYPE = 0, NUMERIC_TYPE = 1};

/**********************************************************************/
/** MLNetwork components **********************************************/
/**********************************************************************/

// MLNetwork component identifiers

/** A generic identifier for all objects in a MLNetwork (nodes, edges, ...) */
typedef long object_id;
/** The unique identifier of each node inside a MLNetwork */
typedef object_id node_id;
/** The unique identifier of each edge inside a MLNetwork */
typedef object_id edge_id;
/** The unique identifier of each layer in a MLNetwork. Every node belongs to exactly one layer */
typedef object_id  layer_id;
/** Nodes in different layers may correspond to the same "actor",
 * e.g., a person (actor) can have multiple accounts (nodes) on
 * different social media (layers) */
typedef object_id actor_id;

// MLNetwork component objects

/**
 * A generic basic component in a MLNetwork.
 */
class basic_component {
protected:
	/** Constructor */
	basic_component(const object_id& id);
	/** Output function, presenting a complete description of the node */
	std::string to_string() const;
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
 * A basic component of a MLNetwork, which can be identified by name.
 */
class named_component : public basic_component {
protected:
	/** Constructor */
	named_component(const object_id& id, const std::string& name);
	/** Output function, presenting a complete description of the actor */
	std::string to_string() const;
public:
	/** Unique name of the component */
	const std::string name;
};

/**
 * An actor in a MLNetwork.
 */
class actor : public named_component {
public:
	/** Constructor */
	actor(const actor_id& id, const std::string& name);
	/** Output function, presenting a complete description of the actor */
	std::string to_string() const;
};

/** A smart pointer to objects of type actor */
typedef std::shared_ptr<actor> ActorSharedPtr;

/**
 * A layer in a MLNetwork.
 */
class layer : public named_component {
public:
	/** Constructor */
	layer(const layer_id& id, const std::string& name);
	/** Output function, presenting a complete description of the layer */
	std::string to_string() const;
};

/** A smart pointer to objects of type layer */
typedef std::shared_ptr<layer> LayerSharedPtr;

/**
 * A node inside a MLNetwork.
 */
class node : public basic_component {
public:
	/** The actor corresponding to this node */
	ActorSharedPtr actor;
	/** The layer where this node is located */
	LayerSharedPtr layer;
	/** Constructor */
	node(const node_id& id, const ActorSharedPtr& actor, const LayerSharedPtr& layer);
	/** Output function, presenting a complete description of the node */
	std::string to_string() const;
};

/** A smart pointer to objects of type node */
typedef std::shared_ptr<node> NodeSharedPtr;

/**
 * An edge between two nodes in a MLNetwork.
 */
class edge : public basic_component  {
public:
	/** The node at the first end of this edge */
	NodeSharedPtr v1;
	/** The node at the second end of this edge */
	NodeSharedPtr v2;
	/** Edge directionality */
	bool directed;
	/** Constructor */
	edge(const edge_id& id, const NodeSharedPtr& v1, const NodeSharedPtr& v2, bool directed);
	/** Output function, presenting a complete description of the edge */
	std::string to_string() const;
};

/** A smart pointer to objects of type edge */
typedef std::shared_ptr<edge> EdgeSharedPtr;

/**********************************************************************/
/** Sorted Set ********************************************************/
/**********************************************************************/

/**
 * A SortedSet is a class used to store a set of components that:
 * 1. can be accessed by id in log time.
 * 2. can be accessed by index (position) in the set in constant time.
 * 3. can be accessed using a const iterator, so that it is not necessary to duplicate them. As an example,
 * it is not necessary to duplicate nodes when the neighbors of a node are accessed: a SortedSet on those
 * nodes is returned instead.
 *
 * A SortedSet is implemented as a skip list.
 */
template <class T> class SortedSet;

/**
 * An entry in a SortedSet, which is implemented as a skip list.
 */
template <class T>
class Entry {
	friend class SortedSet<T>;

private:
	/** The id of the object corresponding to this entry */
	object_id id;
	/** The object corresponding to this entry */
	T obj_ptr;
	/** An array of pointers to the next entry, for each level in the skip list */
    std::vector<Entry<T>*> forward; // array of pointers
    /** The number of entries before the next entry on each level, used for positional access */
    std::vector<int> link_length;

public:
    /**
     * Constructor.
     * @param level height of the entry in the skip list
     * @param id id of the object corresponding to this entry
     * @param obj the object corresponding to this entry
     */
    Entry(int level, object_id id, T obj);
};

/**
 * A SortedSet is a class used to store a set of components that:
 * 1. can be accessed by id in log time.
 * 2. can be accessed by index (position) in the set in constant time.
 * 3. can be accessed using a const iterator, so that it is not necessary to duplicate them. As an example,
 * it is not necessary to duplicate nodes when the neighbors of a node are accessed: a SortedSet on those
 * nodes is returned instead.
 *
 * A SortedSet is implemented as a skip list.
 */
template <class T>
class SortedSet {
private:
	int MAX_LEVEL = 15;
	float P = 0.5;

    Entry<T> *header;
    int level;
    long num_entries=0;

public:
    SortedSet();

    /** Iterator over the obects in this collection */
	class iterator {
	    typedef std::forward_iterator_tag iterator_category;
		public:
		iterator();
		/** Returns an iterator pointing at the input object */
		iterator(Entry<T>* iter);
		/** Return the object pointed by this iterator */
		T operator*();
		/** Moves the iterator to the next object in the collection (prefix) */
		iterator operator++();
		/** Moves the iterator to the next object in the collection (postfix) */
		iterator operator++(int);
		/** Checks if this iterator equals the input one */
	    bool operator==(const SortedSet<T>::iterator& rhs);
		/** Checks if this iterator differs from the input one */
	    bool operator!=(const SortedSet<T>::iterator& rhs);
		private:
		/** Entry currently pointed to by this iterator */
		Entry<T>* current;
	};
	/** Returns an iterator to the first object in the collection */
	SortedSet<T>::iterator begin() const;
	/** Returns an iterator after the last object in the collection */
	SortedSet<T>::iterator end() const;
	/** Returns the number of objects in the collection */
    long size() const;
	/** Returns true if an obect with the input id is present in the collection */
    bool contains(object_id) const;
	/** Returns the obect with the input id if it is present in the collection, or NULL */
    T get(object_id) const;
	/** Returns the obect at the given position in the collection, or NULL */
    T get_at_index(long) const;
	/** Inserts a new object in the collection */
    void insert(object_id,T);
	/** Removes the input object from the collection */
    void erase(object_id);
};

#include "sortedset.cpp"

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
	Attribute(const std::string& name, attribute_type type);

	/**
	 * @brief Returns the name of the attribute
	 * @return the name of the attribute.
	 **/
	const std::string& name() const;

	/**
	 * @brief Returns the type of the attribute.
	 * @return the type of the attribute.
	 **/
	int type() const;

	/**
	 * @brief Returns a string representation of the type of the attribute.
	 * @return a string representation of the type of the attribute.
	 **/
	std::string type_as_string() const;

private:
	std::string aname;
	attribute_type atype;
};

/** A smart pointer to objects of type Attribute */
typedef std::shared_ptr<Attribute> AttributeSharedPtr;

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
	 * @brief Returns the number of attributes in this store, of all types.
	 * @return the number of attributes in this store
	 **/
	int numAttributes() const;

	/**
	 * @brief Returns the attributes in this store.
	 * @return a vector containing all the attributes.
	 **/
	const std::vector<AttributeSharedPtr>& attributes() const;

	/**
	 * @brief Returns the n^th attribute in this store.
	 * @param idx the position of the attribute (from 0 to numAttributes()-1).
	 * @return an object of type Attribute, or NULL if idx's attribute does not exist.
	 **/
	AttributeSharedPtr attribute(int idx) const;

	/**
	 * @brief Returns an attribute by name.
	 * @param name The name of the queried attribute.
	 * @return an object of type Attribute, or NULL if an attribute with this name does not exist.
	 **/
	AttributeSharedPtr attribute(const std::string& name) const;

	/**
	 * @brief Enables the association of a value to each object in this store.
	 * @param attribute_name The name of the attribute
	 * @param type The type of the attribute
	 * STRING_TYPE: c++ "std::string" type
	 * NUMERIC_TYPE: c++ "double" type
	 * @throws DuplicateElementException if an attribute with this name already exists
	 **/
	void add(const std::string& attribute_name, attribute_type type);

	/**
	 * @brief Sets the value of an attribute.
	 * @param id the id of the object whose associated value is set
	 * @param attribute_name The name of the attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no attribute with this name
	 * @throws OperationNotSupportedException if the attribute type is not STRING_TYPE
	 **/
	void setString(const object_id& id, const std::string& attribute_name, const std::string& value);

	/**
	 * @brief Sets the value of an attribute.
	 * @param id the id of the object whose associated value is set
	 * @param attribute_name The name of the attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no attribute with this name
	 * @throws OperationNotSupportedException if the attribute type is not NUMERIC_TYPE
	 **/
	void setNumeric(const object_id& id, const std::string& attribute_name, double value);

	/**
	 * @brief Gets the value of an attribute.
	 * @param id the id of the object whose associated value is retrieved
	 * @param attribute_name The name of the attribute
	 * @return The value associated to the object, or null if the object id has not been registered in this store
	 * @throws ElementNotFoundException if there is no attribute with this name
	 **/
	const std::string& getString(const object_id& id, const std::string& attribute_name) const;

	/**
	 * @brief Gets the value of an attribute.
	 * @param id the id of the object whose associated value is retrieved
	 * @param attribute_name The name of the attribute
	 * @throws ElementNotFoundException if there is no object with this id
	 **/
	const double& getNumeric(const object_id& id, const std::string& attribute_name) const;

	/**
	 * @brief Removes all the attribute values from an object.
	 * If the same object is queried after this method has been called, default values will be returned.
	 * @param id The id of the object to be removed from the store.
	 **/
	void remove(const object_id& id);

public:
	/** default value for numeric attributes */
	double default_numeric = 0.0;
	/** default value for string attributes */
	std::string default_string = "";
private:
	/* meta-data: names and types of attributes */
	std::vector<AttributeSharedPtr> attribute_vector;
	std::map<std::string,int> attribute_ids;
	/* These maps are structured as: map[AttributeName][object_id][AttributeValue] */
	std::map<std::string, std::map<object_id, std::string> > string_attribute;
	std::map<std::string, std::map<object_id, double> > numeric_attribute;
};

/** A smart pointer to objects of type AttributeStore */
typedef std::shared_ptr<AttributeStore> AttributeStoreSharedPtr;

/**********************************************************************/
/** MLNetwork *********************************************************/
/**********************************************************************/

/**
 * Main data structure of the package, defining a multilayer network.
 */
class MLNetwork {
	friend class actor_list;
	friend class layer_list;
	friend class node_list;
	friend class edge_list;

	/**************************************************************************************
	 * Constructors/destructors
	 **************************************************************************************/
private:
	/**
	 * Creates an empty MLNetwork.
	 * @param name name of the new MLNetwork
	 */
	MLNetwork(const std::string& name);

public:

	/**
	 * Creates an empty MLNetwork and returns a pointer to it. This is the only way to create a network,
	 * so that it cannot be duplicated.
	 * @param name name of the new multilayer network
	 * @return a pointer to the new multilayer network
	 */
	static MLNetworkSharedPtr create(const std::string& name);

	/**************************************************************************************
	 * Basic structural operations used to modify a MLNetwork
	 **************************************************************************************/

	/**
	 * @brief Adds a new actor to the MLNetwork.
	 * A new identifier is automatically associated to the new actor.
	 * @param name name of the actor
	 * @return a pointer to the new actor
	 * @throws DuplicateElementException if the actor is already present in the network
	 **/
	ActorSharedPtr add_actor(const std::string& name);

	/**
	 * @brief Returns an actor by ID.
	 * This method can also be used to check if an actor is present in the MLNetwork.
	 * @param id identifier of the actor
	 * @return a pointer to the requested actor, or null if the actor does not exist
	 **/
	ActorSharedPtr get_actor(const actor_id& id) const;

	/**
	 * @brief Returns an actor by name.
	 * This function can also be used to check if an actor is present in the MLNetwork.
	 * @param name name of the actor
	 * @return a pointer to the requested actor, or null if the actor does not exist
	 **/
	ActorSharedPtr get_actor(const std::string& name) const;

	/**
	 * @brief Returns all the actors in the MLNetwork.
	 * @return a pointer to an actor iterator
	 **/
	const SortedSet<ActorSharedPtr>& get_actors() const;

	/**
	 * @brief Sets the default edge directionality depending on the layers of the connected nodes.
	 * @param layer1 a pointer to the "from" layer
	 * @param layer2 a pointer to the "to" layer
	 * @param directed TRUE or FALSE
	 * @return a pointer to the new layer
	 **/
	void set_directed(LayerSharedPtr layer1, LayerSharedPtr layer2, bool directed);

	/**
	 * @brief Gets the default edge directionality depending on the layers of the connected nodes.
	 * @return a Boolean value indicating if edges among these two layers are directed or not.
	 **/
	bool is_directed(LayerSharedPtr layer1, LayerSharedPtr layer2) const;

	/**
	 * @brief Adds a new layer to the MLNetwork.
	 * A new identifier is automatically associated to the new layer
	 * @param name name of the layer
	 * @param directed TRUE or FALSE
	 * @return a pointer to the new layer
	 * @throws DuplicateElementException if the layer is already present in the network
	 **/
	LayerSharedPtr add_layer(const std::string& name, bool directed);

	/**
	 * @brief Returns a layer by ID.
	 * This function can also be used to check if a layer is present in the MLNetwork
	 * @param id identifier of the layer
	 * @return a pointer to the requested layer, or null if the layer does not exist
	 **/
	LayerSharedPtr get_layer(const layer_id& id) const;

	/**
	 * @brief Returns a layer by name.
	 * This function can also be used to check if a layer is present in the MLNetwork
	 * @param name name of the layer
	 * @return a pointer to the requested layer, or null if the layer does not exist
	 **/
	LayerSharedPtr get_layer(const std::string& name) const;

	/**
	 * @brief Returns all the layers in the MLNetwork.
	 * @return a layer iterator
	 **/
	const SortedSet<LayerSharedPtr>& get_layers() const;

	/**
	 * @brief Adds a new node to the MLNetwork.
	 * A new identifier and a default name are automatically associated to the new node
	 * @param layer pointer to the layer where this node is located
	 * @param actor pointer to the actor corresponding to this node
	 * @return a pointer to the new node
	 * @throws ElementNotFoundException if the input layer or actor are not present in the network
	 **/
	NodeSharedPtr add_node(const ActorSharedPtr& actor, const LayerSharedPtr& layer);

	/**
	 * @brief Adds a new node to the MLNetwork.
	 * A new identifier is automatically associated to the new node
	 * @param layer pointer to the layer where this node is located
	 * @param actor pointer to the actor corresponding to this node
	 * @param name name of the node
	 * @return a pointer to the new node
	 * @throws ElementNotFoundException if the input layer or actor are not present in the network
	 **/
	NodeSharedPtr add_node(const std::string& name, const ActorSharedPtr& actor, const LayerSharedPtr& layer);

	/**
	 * @brief Returns a node by ID.
	 * This function can also be used to check if a node is present in the MLNetwork
	 * @param id identifier of the layer
	 * @return a pointer to the requested node, or null if the node does not exist
	 **/
	NodeSharedPtr get_node(const node_id& id) const;

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
	const SortedSet<NodeSharedPtr>& get_nodes() const;

	/**
	 * @brief Returns all the nodes in a layer.
	 * @param layer pointer to the layer where this node is located
	 * @return a node iterator
	 **/
	const SortedSet<NodeSharedPtr>& get_nodes(const LayerSharedPtr& layer) const;

	/**
	 * @brief Returns the nodes associated to the input actor.
	 * @param actor pointer to the actor
	 * @return an iterator containing pointers to nodes
	 **/
	const SortedSet<NodeSharedPtr>& get_nodes(const ActorSharedPtr& actor) const;

	/**
	 * @brief Adds a new edge to the MLNetwork.
	 * Multiple edges between the same pair of nodes are not allowed. The directionality of the
	 * edge is defined by the layers of the two nodes.
	 * @param node1 a pointer to the "from" node if directed, or to one end of the edge if undirected
	 * @param node2 a pointer to the "to" node if directed, or one end of the edge if undirected
	 * @return a pointer to the new edge
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
	const SortedSet<EdgeSharedPtr>& get_edges() const;

	/**
	 * @brief Returns all the edges from a layer A to a layer B.
	 * This method can also retrieve intra-layer edges setting A = B.
	 * @param layer1 pointer to the layer where the first ends of the edges are located
	 * @param layer2 pointer to the layer where the second ends of the edges are located
	 * @return an edge iterator
	 **/
	const SortedSet<EdgeSharedPtr>& get_edges(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) const;

	/**
	 * @brief Deletes an existing node.
	 * All related data, including node attributes and edges involving this node, are also deleted.
	 * @param node a pointer to the node to be deleted
	 **/
	void erase(const NodeSharedPtr& node);

	/**
	 * @brief Deletes an existing edge.
	 * Attribute values associated to this edge are also deleted.
	 * @param edge a pointer to the edge to be deleted
	 **/
	void erase(const EdgeSharedPtr& edge);

	// deleteActor and deleteLayer are not considered necessary

	/*****************************************************************************************
	 * Functions returning information about the MLNetwork.
	 *****************************************************************************************/

	/**
	 * @brief Returns the nodes with an edge from/to the input node.
	 * @param node pointer to the node
	 * @param mode IN, OUT or INOUT
	 * @return an iterator containing pointers to nodes
	 * @throws WrongParameterException if mode is not one of IN, OUT or INOUT
	 **/
	const SortedSet<NodeSharedPtr>& neighbors(const NodeSharedPtr& node, int mode) const;

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

	/** Returns a string represenation of this MLNetwork */
	std::string to_string() const;

	/** Name of the multilayer network */
	const std::string name;

private:
	// largest identifier assigned so far
	node_id max_node_id;
	edge_id max_edge_id;
	actor_id max_actor_id;
	layer_id max_layer_id;

	/* Edge directionality */
	std::map<layer_id, std::map<layer_id, bool> > edge_directionality;

	// Components:
	SortedSet<LayerSharedPtr> layers;
	SortedSet<ActorSharedPtr> actors;
	SortedSet<NodeSharedPtr> nodes;
	SortedSet<EdgeSharedPtr> edges;

	// Indexes to components:
	std::map<std::string, LayerSharedPtr> cidx_layer_by_name;
	std::map<std::string, ActorSharedPtr> cidx_actor_by_name;
	std::map<actor_id, std::map<layer_id, NodeSharedPtr > > cidx_node_by_actor_and_layer;
	std::map<node_id, std::map<node_id, EdgeSharedPtr> > cidx_edge_by_nodes;

	// Indexes to sets of components:
	std::map<layer_id, SortedSet<NodeSharedPtr> > sidx_nodes_by_layer;
	std::map<actor_id, SortedSet<NodeSharedPtr> > sidx_nodes_by_actor;
	std::map<layer_id, std::map<layer_id, SortedSet<EdgeSharedPtr> > > sidx_edges_by_layer_pair;

	std::map<node_id, SortedSet<NodeSharedPtr> > sidx_neighbors_out;
	std::map<node_id, SortedSet<NodeSharedPtr> > sidx_neighbors_in;
	std::map<node_id, SortedSet<NodeSharedPtr> > sidx_neighbors_all;

	/* objects storing the feature vectors of the different components */
	AttributeStoreSharedPtr actor_attributes;
	AttributeStoreSharedPtr layer_attributes;
	std::map<layer_id, AttributeStoreSharedPtr> node_attributes;
	std::map<layer_id, std::map<layer_id, AttributeStoreSharedPtr> > edge_attributes;

};


/**********************************************************************/
/** Distance **********************************************************/
/**********************************************************************/

//class Distance {
//private:
//	const MultipleNetwork *mnet;
//	std::vector<long> num_edges_per_layer;
//	long timestamp;
//	long num_steps;
//
//	friend std::ostream& operator<<(std::ostream &strm, const Distance &dist);
//
//public:
//	/**
//	 * @brief Constructs a
//	 * @throws ElementNotFoundException if the input elements are not present in the network
//	 * @throws OperationNotSupportedException if the two nodes lay on the same network
//	 **/
//	Distance(const Distance& p, long timestamp);
//	Distance(const MultipleNetwork& mnet, long timestamp);
//	Distance(long num_layers, long timestamp);
//	virtual ~Distance();
//
//	long getNumNetworks() const;
//
//	long getTimestamp() const;
//
//	long getNumEdgesOnNetwork(long layer) const;
//
//	void extend(network_id nid);
//
//	bool operator<(const Distance& other) const;
//
//	Distance operator=(const Distance& other) const;
//
//	bool same(const Distance& other) const;
//
//	long length() const;
//};

/**********************************************************************/
/** Path **************************************************************/
/**********************************************************************/

class Path {
private:
	const MLNetworkSharedPtr mnet;
	//std::vector<long> num_edges_per_layer;
	std::vector<EdgeSharedPtr> path;
	NodeSharedPtr origin;
	//long timestamp;

public:
	Path(const MLNetworkSharedPtr mnet, NodeSharedPtr origin);
	/*Path(const Path& p, long timestamp);
	Path(const MLNetwork& mnet, long timestamp);
	Path(long num_layers, long timestamp);
	*/
	~Path();

	NodeSharedPtr begin();

	NodeSharedPtr end();

	//long getTimestamp() const;

	//long getNumNetworks() const;

	//long num_steps(LayerSharedPtr from_layer, LayerSharedPtr to_layer) const;

	//void start(node_id first);

	void step(EdgeSharedPtr e);

	EdgeSharedPtr get_step(long pos) const;

	long length() const;

	bool operator<(const Path& other) const;

	Path operator=(const Path& other) const;

	bool operator==(const Path& other) const;

};

} // namespace mlnet

#endif /* MLNET_DATASTRUCTURES_H_ */
