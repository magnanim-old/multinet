/*
 * datastructures.h
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 0.0.1
 *
 * This file defines the basic data structures composing networks ("vertex_id" and
 * "edge_id") and multiple networks ("global_vertex_id" and "global_edge_id", where
 * "global" indicates that they are not defined on a single network but on the whole
 * multiple network structure). The same global vertex (at the multiple network level)
 * can correspond to multiple vertexes inside different networks.
 *
 * The two main classes defined in this file are Network and MultipleNetwork.
 *
 * A class representing a path between two global
 * vertexes, where the path can traverse multiple networks, are also defined here.
 *
 * BASIC TYPES: network_id, vertex_id, edge_id, global_vertex_id, global_edge_id
 * CLASSES: Network, Multiple Network, Path
 */

#ifndef MULTIPLENETWORK_DATASTRUCTURES_H_
#define MULTIPLENETWORK_DATASTRUCTURES_H_

#include "types.h"
#include <string>
#include <map>
#include <set>
#include <vector>

/** The identifier of a network inside a MultipleNetwork data structure */
typedef int network_id;

/** The identifier of a vertex inside a (single) network */
typedef long vertex_id;

/**
 * The identifier of an edge inside a (single) network.
 * An edge is identified by the two nodes it connects and
 * its directionality. Directionality is used to compute
 * the equality operator: if an edge is undirected, (u,v)==(v,u)
 * (i.e., the two edge_ids refer to the same edge: they are equal)
 */
struct edge_id {
public:
	vertex_id v1;
	vertex_id v2;
	bool directed;
	/** Constructor */
	edge_id(vertex_id v1, vertex_id v2, bool directed);
	/** Comparison operators, to use edge ids as keys in maps */
	bool operator==(const edge_id& e2) const;
	bool operator!=(const edge_id& e2) const;
	bool operator<(const edge_id& e2) const;
};

/**
 * The identifier of a vertex inside a multiple network. This
 * may correspond to different (local) vertex identifiers inside single
 * networks
 */
typedef long global_vertex_id;

/**
 * The identifier of an edge between two global vertexes.
 * An edge is identified by the two nodes it connects and
 * its directionality. Directionality is used to compute
 * the equality operator: if an edge is undirected, (u,v)==(v,u)
 * (i.e., the two edge_ids refer to the same edge: they are equal).
 * The specific network where this edge exist must also be specified
 * (for example, Matteo and Luca can be connected by an indirected
 * edge on the Twitter network - this corresponding to an edge between
 * the "matmagnani" and "LR" vertexes of the Twitter network, mapped
 * to the "global" vertexes Matteo and Luca).
 */
struct global_edge_id {
public:
	global_vertex_id v1;
	global_vertex_id v2;
	network_id network;
	bool directed;
	/* Constructor */
	global_edge_id(global_vertex_id v1, global_vertex_id v2, bool directed, network_id network);
	/* Comparison operators, to use edge ids as keys in maps */
	bool operator==(const global_edge_id& e2) const;
	bool operator!=(const global_edge_id& e2) const;
	bool operator<(const global_edge_id& e2) const;
};

/**********************************************************************/
/** Network ***********************************************************/
/**********************************************************************/
class Network {
public:
	/****************************/
	/* Constructors/destructors */
	/****************************/
	/** Creates an unnamed, undirected and unweighed empty network */
	Network();
	/** Creates an empty network */
	Network(bool named, bool directed, bool weighted);
	/** */
	~Network();
	/**************************************************************************************
	 * Basic structural operations used to add and delete vertexes and edges.
	 *
	 * Error-checking depends on the operation: adding a new vertex requires that the
	 * vertex does not already exist, adding an edge requires that the two vertexes to
	 * be connected exist. On the contrary, delete operations work also if we try to
	 * delete a non-existing object: in this case the network is not modified and "false"
	 * is returned by the function.
	 *
	 * There are specific operations for named/unnamed and weighed/unweighed networks.
	 *
	 * Each operation automatically adapts to the type of network (directed or undirected).
	 **************************************************************************************/
	/**
	 * @brief Adds a vertex to an unnamed network.
	 * If the network is named, the appropriate insert function taking the vertex name as a parameter must be used instead.
	 * @return the id of the new vertex
	 * @throws OperationNotSupportedException if the network is named
	 **/
	vertex_id addVertex();
	/**
	 * @brief Adds a vertex to the network.
	 * @param vertex_name the name of the vertex, that must be unique.
	 * @return the id of the new vertex, or the id of the existing vertex with the same name.
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws DuplicateElementException if vertex_name already exists
	 **/
	vertex_id addVertex(const std::string& vertex_name);
	/**
	 * @brief Adds a new edge.
	 * The type of edge (directed/undirected) is inherited by the type of network (specified at creation time).
	 * If the network is weighted, a default weight is assigned to it as specified in the settings.h file
	 * @param vertex_id1 the "from" vertex in a directed network, or one end of the edge in an undirected one
	 * @param vertex_id2 the "to" vertex in a directed network, or one end of the edge in an undirected one
	 * @return the ID of the new edge
	 * @throws ElementNotFoundException if the input vertexes are not present in the network
	 * @throws DuplicateElementException if the edge is already present in the network
	 **/
	edge_id addEdge(vertex_id vid1, vertex_id vid2);
	/**
	 * @brief Adds a new edge with an associated double precision weight.
	 * The type of edge (directed/undirected) is inherited by the type of network (specified at creation time).
	 * @param vertex_id1 the "from" vertex in a directed network, or one end of the edge in an undirected one
	 * @param vertex_id2 the "to" vertex in a directed network, or one end of the edge in an undirected one
	 * @param weight
	 * @return the ID of the new edge
	 * @throws OperationNotSupportedException if the network is not weighted
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 * @throws DuplicateElementException if the edge is already present in the network
	 **/
	edge_id addEdge(vertex_id vid1, vertex_id vid2, double weight);
	/**
	 * @brief Adds a new edge.
	 * The type of edge (directed/undirected) is inherited by the type of network (specified at creation time).
	 * If the network is weighted, a default weight is assigned to it as specified in the settings.h file
	 * @param vertex_name1 the "from" vertex in a directed network, or one end of the edge in an undirected one
	 * @param vertex_name2 the "to" vertex in a directed network, or one end of the edge in an undirected one
	 * @return the ID of the new edge
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 * @throws DuplicateElementException if the edge is already present in the network
	 **/
	edge_id addEdge(const std::string& vertex_name1, const std::string& vertex_name2);
	/**
	 * @brief Adds a new edge with an associated double precision weight.
	 * The type of edge (directed/undirected) is inherited by the type of network (specified at creation time).
	 * @param vertex_name1 the "from" vertex in a directed network, or one end of the edge in an undirected one
	 * @param vertex_name2 the "to" vertex in a directed network, or one end of the edge in an undirected one
	 * @param weight
	 * @return the ID of the new edge
	 * @throws OperationNotSupportedException if the network is not weighted
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 * @throws DuplicateElementException if the edge is already present in the network
	 **/
	edge_id addEdge(const std::string& vertex_name1, const std::string& vertex_name2, double weight);
	/**
	 * @brief Deletes an existing vertex.
	 * All related data, including vertex attributes and edges involving this vertex, are also deleted.
	 * Notice that it is possible to delete a vertex by id even if the network is named.
	 * @return false if the vertex is not present in the network
	 **/
	bool deleteVertex(vertex_id vid);
	/**
	 * @brief Deletes an existing vertex.
	 * All related data, including vertex attributes and edges involving this vertex, are also deleted.
	 * @param vertex_name the name of the vertex.
	 * @return false if the vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	bool deleteVertex(const std::string& vertex_name);
	/**
	 * @brief Deletes an existing edge.
	 * Attribute values associated to this edge are also deleted.
	 * Notice that it is possible to delete an edge by vertex ids even if the network is named.
	 * @param vertex_id1 the "from" vertex in a directed network, or one end of the edge in an undirected one
	 * @param vertex_id2 the "to" vertex in a directed network, or one end of the edge in an undirected one
	 * @return false if the edge is not present in the network
	 **/
	bool deleteEdge(vertex_id vid1, vertex_id vid2);
	/**
	 * @brief Deletes an existing edge.
	 * Attribute values associated to this edge are also deleted.
	 * @param vertex_name1 the "from" vertex in a directed network, or one end of the edge in an undirected one
	 * @param vertex_name2 the "to" vertex in a directed network, or one end of the edge in an undirected one
	 * @return false if the edge is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	bool deleteEdge(const std::string& vertex_name1, const std::string& vertex_name2);
	/*****************************************************************************************
	 * Functions checking structural properties of the network.
	 * All these functions are const (i.e., they do not modify the network).
	 *****************************************************************************************/
	/**
	 * @brief Returns true if the vertex is present.
	 **/
	bool containsVertex(vertex_id vid) const;
	/**
	 * @brief Returns true if the vertex is present.
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if the input element is not present in the network
	 **/
	bool containsVertex(std::string vertex_name) const;
	/**
	 * @brief Returns true if the edge is present.
	 * In an undirected network an edge a-b is returned also if it was inserted as b-a.
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 **/
	bool containsEdge(vertex_id vid1, vertex_id vid2) const;
	/**
	 * @brief Returns true if the vertex is present.
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 **/
	bool containsEdge(std::string vertex_name1, std::string vertex_name2) const;
	/*****************************************************************************************
	 * Functions returning information about the network.
	 * All these functions are const (i.e., they do not modify the network).
	 *****************************************************************************************/
	/**
	 * @brief Returns true if the network is directed
	 */
	bool isDirected() const;
	/**
	 * @brief Returns true if the network is weighted
	 */
	bool isWeighed() const;
	/**
	 * @brief Returns true if the network is named
	 */
	bool isNamed() const;
	/**
	 * @brief Returns the name of vertex vertex_id.
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	std::string getVertexName(vertex_id vid) const;
	/**
	 * @brief Returns the id of the vertex with name vertex_name.
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	vertex_id getVertexId(std::string vertex_name) const;
	/**
	 * @brief Returns all the vertex identifiers in the network.
	 **/
	void getVertexes(std::set<vertex_id>& vertexes) const;
	/**
	 * @brief Returns all the edge identifiers in the network.
	 **/
	void getEdges(std::set<edge_id>& edges) const;
	/**
	 * @brief Returns the number of vertexes.
	 **/
	long getNumVertexes() const;
	/**
	 * @brief Returns the number of edges.
	 * In an undirected networks an edge a-b is counted only once (not twice by also considering b-a).
	 **/
	long getNumEdges() const;
	/**
	 * @brief Returns the number of outgoing edges from vertex vid.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getOutDegree(vertex_id vid) const;
	/**
	 * @brief Returns the number of incoming edges from vertex vid.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getInDegree(vertex_id vid) const;
	/**
	 * @brief Returns the number of incoming or outgoing edges from/to vertex vid.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getDegree(vertex_id vid) const;
	/**
	 * @brief Returns the number of outgoing edges from vertex name.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getOutDegree(std::string vertex_name) const;
	/**
	 * @brief Returns the number of incoming edges from vertex name.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getInDegree(std::string vertex_name) const;
	/**
	 * @brief Returns the number of incoming or outgoing edges from/to vertex name.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getDegree(std::string vertex_name) const;
	/**
	 * @brief Returns the identifiers of the vertexes with an edge coming from vertex_id.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	void getOutNeighbors(vertex_id vid, std::set<vertex_id>& neighbors) const;
	/**
	 * @brief Returns the identifiers of the vertexes with an edge going to vertex_id.
	 **/
	void getInNeighbors(vertex_id vid, std::set<vertex_id>& neighbors) const;
	/**
	 * @brief Returns the identifiers of the vertexes with an edge coming from or going to vertex_id.
	 **/
	void getNeighbors(vertex_id vid, std::set<vertex_id>& neighbors) const;
	/**
	 * @brief Returns the names of the vertexes with an edge coming from vertex_name.
	 * Notice that vertexes are stored by id, therefore the unnamed version of this function is faster.
	 * @throws ElementNotFoundException if the input vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	void getOutNeighbors(std::string vertex_name, std::set<std::string>& neighbors) const;
	/**
	 * @brief Returns the name of the vertexes with an edge going to vertex_name.
	 * Notice that vertexes are stored by id, therefore the unnamed version of this function is faster.
	 * @throws ElementNotFoundException if the input vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	void getInNeighbors(std::string vertex_name, std::set<std::string>& neighbors) const;
	/**
	 * @brief Returns the name of the vertexes with an edge coming from or going to vertex_name.
	 * Notice that vertexes are stored by id, therefore the unnamed version of this function is faster.
	 * @throws ElementNotFoundException if the input vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	void getNeighbors(std::string vertex_name, std::set<std::string>& neighbors) const;
	/**********************/
	/* Attribute handling */
	/**********************/
	/**
	 * @brief Returns the weight on this edge.
	 * @throws OperationNotSupportedException if the network is not weighted
	 * @throws ElementNotFoundException if the input edge is not present in the network
	 **/
	double getEdgeWeight(vertex_id vid1, vertex_id vid2) const;
	/**
	 * @brief Sets the weight on this edge.
	 * @throws OperationNotSupportedException if the network is not weighted
	 * @throws ElementNotFoundException if the input edge is not present in the network
	 **/
	void setEdgeWeight(vertex_id vid1, vertex_id vid2, double weight);
	/**
	 * @brief Returns the weight on this edge.
	 * @throws OperationNotSupportedException if the network is not weighted or not named
	 * @throws ElementNotFoundException if the input edge is not present in the network
	 **/
	double getEdgeWeight(std::string vertex_name1, std::string vertex_name2) const;
	/**
	 * @brief Sets the weight of edge eid.
	 * @throws OperationNotSupportedException if the network is not weighted or not named
	 * @throws ElementNotFoundException if the input edge is not present in the network
	 **/
	void setEdgeWeight(std::string vertex_name1, std::string vertex_name2, double weight);
	/**
	 * @brief Enables the association of a string value to each vertex.
	 * @param attribute_name The name of the vertex attribute
	 * @throws DuplicateElementException if a vertex attribute with this name already exists
	 **/
	void newStringVertexAttribute(std::string attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	std::string getStringVertexAttribute(vertex_id vid, std::string attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	std::string getStringVertexAttribute(std::string vertex_name,
			std::string attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	void setStringVertexAttribute(vertex_id vid, std::string attribute_name, std::string value);
	/**
	 * @brief Sets the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is set
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	void setStringVertexAttribute(std::string vertex_name, std::string attribute_name, std::string value);
	/**
	 * @brief Enables the association of a numeric (double precision) value to each vertex.
	 * @param attribute_name The name of the vertex attribute
	 * @throws DuplicateElementException if a vertex attribute with this name already exists
	 **/
	void newNumericVertexAttribute(std::string attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	double getNumericVertexAttribute(vertex_id vid, std::string attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	double getNumericVertexAttribute(std::string vertex_name, std::string attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	void setNumericVertexAttribute(vertex_id vid, std::string attribute_name, double value);
	/**
	 * @brief Sets the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is set
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	void setNumericVertexAttribute(std::string vertex_name, std::string attribute_name, double value);
	/**
	 * @brief Enables the association of a string value to each edge.
	 * @param attribute_name The name of the vertex attribute
	 * @throws DuplicateElementException if an edge attribute with this name already exists
	 **/
	void newStringEdgeAttribute(std::string attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	std::string getStringEdgeAttribute(vertex_id vid1, vertex_id vid2, std::string attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	std::string getStringEdgeAttribute(std::string vertex_name1, std::string vertex_name2, std::string attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	void setStringEdgeAttribute(vertex_id vid1, vertex_id vid2, std::string attribute_name, std::string value);
	/**
	 * @brief Sets the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is set
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	void setStringEdgeAttribute(std::string vertex_name1, std::string vertex_name2, std::string attribute_name, std::string value);
	/**
	 * @brief Enables the association of a numeric (double precision) value to each edge.
	 * @param attribute_name The name of the vertex attribute
	 * @throws DuplicateElementException if an edge attribute with this name already exists
	 **/
	void newNumericEdgeAttribute(std::string attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	double getNumericEdgeAttribute(vertex_id vid1, vertex_id vid2,
			std::string attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	double getNumericEdgeAttribute(std::string vertex_name1, std::string vertex_name2, std::string attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	void setNumericEdgeAttribute(vertex_id vid1, vertex_id vid2, std::string attribute_name, double value);
	/**
	 * @brief Sets the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is set
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	void setNumericEdgeAttribute(std::string vertex_name1, std::string vertex_name2, std::string attribute_name, double value);
private:
	// largest vertex identifier assigned so far
	vertex_id max_vertex_id;
	long num_edges;
	bool is_named, is_directed, is_weighed;
	// Set of vertex ids of all vertexes in the network, used for both named and unnamed networks. (redundant in case of named networks)
	std::set<vertex_id> vertexes;
	/* edges (in an undirected network these two maps contain the same values) */
	std::map<vertex_id, std::set<vertex_id> > out_edges;
	std::map<vertex_id, std::set<vertex_id> > in_edges;
	/* Conversion from numerical to string ids */
	std::map<vertex_id, std::string> vertex_id_to_name;
	std::map<std::string, vertex_id> vertex_name_to_id;
	/* Attributes. All these maps are structured as: map[AttributeName][vertex_or_edge_id][AttributeValue] */
	std::map<std::string, std::map<vertex_id, std::string> > vertex_string_attribute;
	std::map<std::string, std::map<edge_id, std::string> > edge_string_attribute;
	std::map<std::string, std::map<vertex_id, double> > vertex_numeric_attribute;
	std::map<std::string, std::map<edge_id, double> > edge_numeric_attribute;
};

void print(Network& net);

/**********************************************************************/
/** Multiple Network **************************************************/
/**********************************************************************/
class MultipleNetwork {
public:
	/** Creates an empty network (with 0 internal networks) */
	MultipleNetwork();
	/** */
	~MultipleNetwork();
	/**
	 * @brief Adds a global vertex to the network.
	 * Global vertexes are global identifiers used to relate vertexes in the local networks.
	 * For example, a local node "1" in network "0" and a local node "13" in network "1" may refer
	 * to the same global vertex.
	 * @return the vertex identifier of the new global vertex
	 **/
	global_vertex_id addVertex();
	/**
	 * @brief Adds a named global vertex to the network.
	 * Global vertexes are global identifiers used to relate vertexes in the local networks.
	 * For example, a local node "v1" in network "v0" and a local node "v13" in network "v1" may refer
	 * to the same global vertex.
	 * @return the vertex identifier of the new global vertex
	 **/
	global_vertex_id addVertex(std::string name);
	/**
	 * @brief Adds num_new_vertexes global vertexes to the network.
	 * Global vertexes are global identifiers used to relate vertexes in the local networks.
	 * For example, a local node "1" in network "0" and a local node "13" in network 1 may refer
	 * to the same global vertex.
	 * When global identities have been created they cannot be deleted. However, a global vertex may end up
	 * having no corresponding local vertexes in any local network.
	 **/
	void addVertexes(long num_new_vertexes);
	/**
	 * @brief Adds a local network to this multiplenetwork data structure.
	 * @return the identifier of the newly added network
	 **/
	network_id addNetwork(Network& net);
	/**
	 * @brief Adds a named local network to this multiplenetwork data structure.
	 * @return the identifier of the newly added network
	 **/
	network_id addNetwork(std::string network_name, Network& net);
	/**
	 * @brief Defines that vertex lvid in network nid corresponds to global_vertex_id gvid.
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 **/
	void map(global_vertex_id gvid, vertex_id lvid, network_id nid);
	/**
	 * @brief Defines that vertex local_vertex_name in network network_name corresponds to global vertex global_vertex_name.
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 **/
	void map(std::string global_vertex_name, std::string local_vertex_name, std::string network_name);
	/**
	 * @brief Inserts into "networks" all the network identifiers.
	 **/
	void getNetworks(std::set<network_id>& networks);
	/**
	 * @brief Inserts into "vertexes" all the (global) vertex identifiers.
	 **/
	void getVertexes(std::set<global_vertex_id>& vertexes);
	/**
	 * @brief Inserts into "edges" all the (global) edge identifiers.
	 **/
	void getEdges(std::set<global_edge_id>& edges);
	/**
	 * @brief Returns the local vertex identifier in network nid corresponding to global vertex gvid.
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	vertex_id getLocalVertexId(global_vertex_id gvid, network_id nid);
	/**
	 * @brief Returns the local vertex name in network network_name corresponding to global vertex global_vertex_name.
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	std::string getLocalVertexName(std::string global_vertex_name, std::string network_name);
	/**
	 * @brief Finds the global identifier corresponding to vertex lvid in local network nid.
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	global_vertex_id getGlobalVertexId(vertex_id lvid, network_id nid);
	/**
	 * @brief Finds the global identifier corresponding to vertex local_vertex_name in local network network_name.
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	std::string getGlobalVertexName(std::string local_vertex_name, std::string network_name);
	/**
	 * @brief Returns the number of networks.
	 **/
	int getNumNetworks();
	/**
	 * @brief Returns the number of global vertexes.
	 **/
	long getNumVertexes();
	/**
	 * @brief Returns the number of edges.
	 * This function corresponds to computing the sum of the number of edges in each single network.
	 * In an undirected networks an edge a-b is counted only once (not twice by also considering b-a). This can create confusion
	 * when some local networks are directed and some are undirected.
	 **/
	long getNumEdges();
	/**
	 * @brief Returns a pointer to the network with identifier nid.
	 **/
	Network* getNetwork(network_id nid);
	/**
	 * @brief Returns a pointer to the network network_name.
	 **/
	Network* getNetwork(std::string network_name);
	/**
	 * @brief Checks if there is a global vertex with identifier gvid.
	 **/
	bool containsVertex(global_vertex_id gvid);
	/**
	 * @brief Checks if there is a global vertex global_vertex_name.
	 **/
	bool containsVertex(std::string global_vertex_name);
	/**
	 * @brief Checks if global vertex gvid is associated to a vertex in network nid.
	 **/
	bool containsVertex(global_vertex_id gvid, network_id nid);
	/**
	 * @brief Checks if global vertex global_vertex_name is associated to a vertex in network network_name.
	 **/
	bool containsVertex(std::string global_vertex_name, std::string network_name);
	/**
	 * @brief Checks if there is a network with identifier nid.
	 **/
	bool containsNetwork(network_id nid);
	/**
	 * @brief Checks if there is a network network_name.
	 **/
	bool containsNetwork(std::string network_name);
	/* Name-ID mapping functions */
	std::string getNetworkName(network_id nid);
	network_id getNetworkId(std::string network_name);
	std::string getVertexName(global_vertex_id global_vertex_id);
	global_vertex_id getVertexId(std::string global_vertex_name);
private:
	// The networks composing this multiple network system
	std::vector<Network> networks;
	// How to use: local_to_global_id[network_id][local_vertex_id]
	std::vector<std::map<vertex_id, vertex_id> > local_to_global_id;
	// How to use: global_to_local_id[global_vertex_id][network_id]
	std::vector<std::map<network_id, vertex_id> > global_to_local_id;
	// Conversion from symbolic names to numerical ids and back
	std::map<std::string, network_id> network_name_to_id;
	std::vector<std::string> network_id_to_name;
	std::map<std::string, vertex_id> vertex_name_to_id;
	std::vector<std::string> vertex_id_to_name;
};

void print(MultipleNetwork& mnet);

/* Path class to be commented */

class Path {
private:
	const MultipleNetwork *mnet;
	std::vector<long> num_edges_per_layer;
	std::vector<vertex_id> path;
	std::vector<network_id> network;
	long timestamp;

	friend std::ostream& operator<<(std::ostream &strm, const Path &path);

public:
	Path(const Path& p, long timestamp);
	Path(MultipleNetwork& mnet, long timestamp);
	Path(long num_layers, long timestamp);
	virtual ~Path();

	long getTimestamp() const;

	long getNumNetworks() const;

	long getNumEdgesOnNetwork(long layer) const;

	void start(vertex_id first);

	void extend(vertex_id to, network_id nid);

	bool operator<(const Path& other) const;

	Path operator=(const Path& other) const;

	bool same(const Path& other) const;

	long length() const;

	long getVertex(long pos) const;

	network_id getNetwork(long pos) const;

};

/*
class Distance {
public:
	Distance();
	virtual ~Distance();
};
*/

#endif /* MULTIPLENETWORK_DATASTRUCTURES_H_ */
