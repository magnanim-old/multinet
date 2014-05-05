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


/**********************************************************************/
/** Network components ************************************************/
/**********************************************************************/

/** The identifier of a vertex inside a single network */
typedef long vertex_id;

/**
 * The identifier of an edge inside a single network.
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
	bool operator>(const edge_id& e2) const;
	friend std::ostream& operator<<(std::ostream &strm, const edge_id& eid);
	std::string to_string() const;
};

/**********************************************************************/
/** Multilayer Network components *************************************/
/**********************************************************************/

/** The identifier of a network inside a MultilayerNetwork data structure */
typedef int network_id;

/**
 * The global identifier of a vertex inside a multiplex network.
 */
struct global_vertex_id {
public:
	vertex_id vid;
	network_id network;
	bool directed;
	/* Constructor */
	global_vertex_id();
	global_vertex_id(vertex_id vid, network_id network);
	/* Comparison operators, to use edge ids as keys in maps */
	bool operator==(const global_vertex_id& e2) const;
	bool operator!=(const global_vertex_id& e2) const;
	bool operator<(const global_vertex_id& e2) const;
	bool operator>(const global_vertex_id& e2) const;
};

/**
 * The identifier of an edge between two global vertexes
 * in a multilayer network.
 * An edge is identified by the two nodes it connects, the network and
 * its directionality. Directionality is used to evaluate
 * the equality operator: if an edge is undirected, (u,v)==(v,u)
 * (i.e., the two edge_ids refer to the same edge: they are equal).
 */
struct global_edge_id {
public:
	vertex_id v1;
	vertex_id v2;
	network_id network;
	bool directed;
	/* Constructor */
	global_edge_id(vertex_id v1, vertex_id v2, network_id network, bool directed);
	/* Comparison operators, to use edge ids as keys in maps */
	bool operator==(const global_edge_id& e2) const;
	bool operator!=(const global_edge_id& e2) const;
	bool operator<(const global_edge_id& e2) const;
	bool operator>(const global_edge_id& e2) const;
};

/**
 * Specific for Multiplex Networks: vertexes in different networks may be "the same vertex", i.e., be mapped to the same global identity.
 * In this way implicit connections between vertexes in different networks are defined.
 */
typedef long global_identity;

/**
 * Specific for Multiple Interdependent Networks: the identifier of an
 * edge between two vertexes on two (potentially different) networks.
 * An edge is identified by the two global vertexes it connects and
 * its directionality. Directionality is used to compute
 * the equality operator: if an edge is undirected, (u,n1,v,n2)==(v,n2,u,n1)
 * (i.e., the two edge_ids refer to the same edge: they are equal).
 * For example, Matteo on the "Supervisors" friendship network and
 * Luca on the "PhD Students" friendship network can be connected by an
 * edge indicating that Matteo supervises Luca).
 */
struct interlayer_edge_id {
public:
	global_vertex_id v1;
	global_vertex_id v2;
	bool directed;
	/* Constructor */
	interlayer_edge_id(global_vertex_id v1, global_vertex_id v2, bool directed);
	/* Comparison operators, to use edge ids as keys in maps */
	bool operator==(const interlayer_edge_id& e2) const;
	bool operator!=(const interlayer_edge_id& e2) const;
	bool operator<(const interlayer_edge_id& e2) const;
	bool operator>(const interlayer_edge_id& e2) const;
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
	 * @param vid1 the "from" vertex in a directed network, or one end of the edge in an undirected one
	 * @param vid2 the "to" vertex in a directed network, or the other end of the edge in an undirected one
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
	 * If the network is weighted, a default weight is assigned to it as specified in the types.h file
	 * @param vertex_name1 the "from" vertex in a directed network, or one end of the edge in an undirected one
	 * @param vertex_name2 the "to" vertex in a directed network, or the other end of the edge in an undirected one
	 * @return the ID of the new edge
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 * @throws DuplicateElementException if the edge is already present in the network
	 **/
	edge_id addEdge(const std::string& vertex_name1, const std::string& vertex_name2);
	/**
	 * @brief Adds a new edge with an associated double precision weight.
	 * The type of edge (directed/undirected) is inherited by the type of network (specified at creation time).
	 * @param vertex_name1 the "from" vertex in a directed network, or one end of the edge in an undirected one
	 * @param vertex_name2 the "to" vertex in a directed network, or the other end of the edge in an undirected one
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
	bool containsVertex(const std::string& vertex_name) const;
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
	bool containsEdge(const std::string& vertex_name1, const std::string& vertex_name2) const;
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
	vertex_id getVertexId(const std::string& vertex_name) const;
	/**
	 * @brief Returns all the vertex identifiers in the network.
	 * This method is normally used to iterate over all the vertexes
	 * @param vertexes set where the vertex ids are inserted
	 **/
	void getVertexes(std::set<vertex_id>& vertexes) const;
	/**
	 * @brief Returns all the edge identifiers in the network.
	 * This method is normally used to iterate over all the edges
	 * @param edges set where the edge ids are inserted
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
	long getOutDegree(const std::string& vertex_name) const;
	/**
	 * @brief Returns the number of incoming edges from vertex name.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getInDegree(const std::string& vertex_name) const;
	/**
	 * @brief Returns the number of incoming or outgoing edges from/to vertex name.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getDegree(const std::string& vertex_name) const;
	/**
	 * @brief Returns the identifiers of the vertexes with an edge from vid.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	void getOutNeighbors(vertex_id vid, std::set<vertex_id>& neighbors) const;
	/**
	 * @brief Returns the identifiers of the vertexes with an edge towards vid.
	 **/
	void getInNeighbors(vertex_id vid, std::set<vertex_id>& neighbors) const;
	/**
	 * @brief Returns the identifiers of the vertexes with an edge from or towards to vid.
	 **/
	void getNeighbors(vertex_id vid, std::set<vertex_id>& neighbors) const;
	/**
	 * @brief Returns the names of the vertexes with an edge coming from vertex_name.
	 * Notice that vertexes are stored by id, therefore the unnamed version of this function is faster.
	 * @throws ElementNotFoundException if the input vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	void getOutNeighbors(const std::string& vertex_name, std::set<std::string>& neighbors) const;
	/**
	 * @brief Returns the name of the vertexes with an edge going to vertex_name.
	 * Notice that vertexes are stored by id, therefore the unnamed version of this function is faster.
	 * @throws ElementNotFoundException if the input vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	void getInNeighbors(const std::string& vertex_name, std::set<std::string>& neighbors) const;
	/**
	 * @brief Returns the name of the vertexes with an edge coming from or going to vertex_name.
	 * Notice that vertexes are stored by id, therefore the unnamed version of this function is faster.
	 * @throws ElementNotFoundException if the input vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	void getNeighbors(const std::string& vertex_name, std::set<std::string>& neighbors) const;
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
	double getEdgeWeight(const std::string& vertex_name1, const std::string& vertex_name2) const;
	/**
	 * @brief Sets the weight of edge eid.
	 * @throws OperationNotSupportedException if the network is not weighted or not named
	 * @throws ElementNotFoundException if the input edge is not present in the network
	 **/
	void setEdgeWeight(const std::string& vertex_name1, const std::string& vertex_name2, double weight);
	/**
	 * @brief Enables the association of a string value to each vertex.
	 * @param attribute_name The name of the vertex attribute
	 * @throws DuplicateElementException if a vertex attribute with this name already exists
	 **/
	void newStringVertexAttribute(const std::string& attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	std::string getStringVertexAttribute(vertex_id vid, const std::string& attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	std::string getStringVertexAttribute(const std::string& vertex_name, const std::string& attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	void setStringVertexAttribute(vertex_id vid, const std::string& attribute_name, const std::string& value);
	/**
	 * @brief Sets the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is set
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	void setStringVertexAttribute(const std::string& vertex_name, const std::string& attribute_name, const std::string& value);
	/**
	 * @brief Enables the association of a numeric (double precision) value to each vertex.
	 * @param attribute_name The name of the vertex attribute
	 * @throws DuplicateElementException if a vertex attribute with this name already exists
	 **/
	void newNumericVertexAttribute(const std::string& attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	double getNumericVertexAttribute(vertex_id vid, const std::string& attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	double getNumericVertexAttribute(const std::string& vertex_name, const std::string& attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	void setNumericVertexAttribute(vertex_id vid, const std::string& attribute_name, double value);
	/**
	 * @brief Sets the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is set
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	void setNumericVertexAttribute(const std::string& vertex_name, const std::string& attribute_name, double value);
	/**
	 * @brief Enables the association of a string value to each edge.
	 * @param attribute_name The name of the vertex attribute
	 * @throws DuplicateElementException if an edge attribute with this name already exists
	 **/
	void newStringEdgeAttribute(const std::string& attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	std::string getStringEdgeAttribute(vertex_id vid1, vertex_id vid2, const std::string& attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	std::string getStringEdgeAttribute(const std::string& vertex_name1, const std::string& vertex_name2, const std::string& attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	void setStringEdgeAttribute(vertex_id vid1, vertex_id vid2, const std::string& attribute_name, const std::string& value);
	/**
	 * @brief Sets the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is set
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	void setStringEdgeAttribute(const std::string& vertex_name1, const std::string& vertex_name2, const std::string& attribute_name, const std::string& value);
	/**
	 * @brief Enables the association of a numeric (double precision) value to each edge.
	 * @param attribute_name The name of the vertex attribute
	 * @throws DuplicateElementException if an edge attribute with this name already exists
	 **/
	void newNumericEdgeAttribute(const std::string& attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	double getNumericEdgeAttribute(vertex_id vid1, vertex_id vid2, const std::string& attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	double getNumericEdgeAttribute(const std::string& vertex_name1, const std::string& vertex_name2, const std::string& attribute_name) const;
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	void setNumericEdgeAttribute(vertex_id vid1, vertex_id vid2, const std::string& attribute_name, double value);
	/**
	 * @brief Sets the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is set
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	void setNumericEdgeAttribute(const std::string& vertex_name1, const std::string& vertex_name2, const std::string& attribute_name, double value);
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
/** Multilayer Network ************************************************/
/**********************************************************************/
class MultilayerNetwork {
public:
	/** Creates an empty network (with 0 internal networks) */
	MultilayerNetwork();
	/** */
	~MultilayerNetwork();
	/**
	 * @brief Adds a local network to this multiplenetwork data structure.
	 * @return the identifier of the newly added network
	 **/
	network_id addNetwork(const Network& net);
	/**
	 * @brief Adds a named local network to this multiplenetwork data structure.
	 * @return the identifier of the newly added network
	 **/
	network_id addNetwork(const std::string& network_name, const Network& net);
	/**
	 * @brief Returns into "networks" all the network identifiers.
	 **/
	void getNetworks(std::set<network_id>& networks) const;
	/**
	 * @brief Inserts into "vertexes" all the (global) vertex identifiers.
	 **/
	void getVertexes(std::set<global_vertex_id>& vertexes) const;
	/**
	 * @brief Inserts into "edges" all the (global) edge identifiers.
	 **/
	void getEdges(std::set<global_edge_id>& edges) const;
	/**
	 * @brief Returns the number of networks.
	 **/
	int getNumNetworks() const;
	/**
	 * @brief Returns the number of global vertexes.
	 **/
	long getNumVertexes() const;
	/**
	 * @brief Returns the number of global edges.
	 * This function corresponds to computing the sum of the number of edges in each single network.
	 * In an undirected networks an edge a-b is counted only once (not twice by also considering b-a).
	 * N.B. This can create some confusion when some local networks are directed and some are undirected.
	 **/
	long getNumEdges() const;
	/**
	 * @brief Returns a pointer to the network with identifier nid.
	 **/
	Network& getNetwork(network_id nid);
	/**
	 * @brief Returns a pointer to the network with identifier nid (const version).
	 **/
	const Network& getNetwork(network_id nid) const;
	/**
	 * @brief Returns a pointer to the network network_name.
	 **/
	Network& getNetwork(const std::string& network_name);
	/**
	 * @brief Checks if there is a global vertex with identifier gvid.
	 **/
	bool containsVertex(global_vertex_id gvid) const;
	/**
	 * @brief Checks if there is a network with identifier nid.
	 **/
	bool containsNetwork(network_id nid) const;
	/**
	 * @brief Checks if there is a network network_name.
	 **/
	bool containsNetwork(const std::string& network_name) const;
	/**
	 * @brief Returns the name of the network with identifier nid.
	 * @throws ElementNotFoundException if the network is not present
	 **/
	std::string getNetworkName(network_id nid) const;
	/**
	 * @brief Returns the identifier of the network with the input name.
	 * @throws ElementNotFoundException if the network is not present
	 **/
	network_id getNetworkId(const std::string& network_name) const;
private:
	// The networks composing this multiple network system
	std::vector<Network> networks;
	// Conversion from symbolic names to numerical ids and back
	std::map<std::string, network_id> network_name_to_id;
	std::vector<std::string> network_id_to_name;
};

void print(MultilayerNetwork& mnet);

/**********************************************************************/
/** Multiplex Network *************************************************/
/**********************************************************************/
class Multiplex : public MultilayerNetwork {
public:
	/** Creates an empty network (with 0 internal networks) */
	Multiplex();
	/** */
	~Multiplex();
	/**
	 * @brief Adds a global vertex to the network.
	 * Global vertexes are global identifiers used to relate vertexes in the local networks.
	 * For example, a local node "1" in network "0" and a local node "13" in network "1" may refer
	 * to the same global vertex.
	 * @return the vertex identifier of the new global vertex
	 **/
	global_identity addGlobalIdentity();
	/**
	 * @brief Adds a named global vertex to the network.
	 * Global vertexes are global identifiers used to relate vertexes in the local networks.
	 * For example, a local node "v1" in network "v0" and a local node "v13" in network "v1" may refer
	 * to the same global vertex.
	 * @return the vertex identifier of the new global vertex
	 **/
	global_identity addGlobalName(const std::string& name);
	/**
	 * @brief Adds num_new_vertexes global identity to the network.
	 * Global vertexes are global identifiers used to relate vertexes in the local networks.
	 * For example, a local node "1" in network "0" and a local node "13" in network 1 may refer
	 * to the same global vertex.
	 * When global identities have been created they cannot be deleted. However, a global vertex may end up
	 * having no corresponding local vertexes in any local network.
	 **/
	void addGlobalIdentities(long num_new_identities);
	/**
	 * @brief Returns the number of global identities.
	 **/
	long getNumGlobalIdentities() const;
	/**
	 * @brief Defines that vertex lvid in network nid corresponds to global_vertex_id gvid.
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 **/
	void mapIdentity(global_identity gvid, vertex_id lvid, network_id nid);
	/**
	 * @brief Defines that vertex local_vertex_name in network network_name corresponds to global vertex global_vertex_name.
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 **/
	void mapIdentity(const std::string& global_vertex_name, const std::string& local_vertex_name, const std::string& network_name);
	/**
	 * @brief Inserts into "identities" all the global identities.
	 **/
	void getGlobalIdntities(std::set<global_identity>& identities) const;
	/**
	 * @brief Returns the name of vertex vertex_id.
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	std::string getGlobalName(global_identity gid) const;
	/**
	 * @brief Returns the id of the vertex with name vertex_name.
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	vertex_id getGlobalIdentity(const std::string& identity_name) const;
	/**
	 * @brief Returns all the vertex identifiers in the network.
	 * This method is normally used to iterate over all the vertexes
	 * @param vertexes set where the vertex ids are inserted
	 **/
	/**
	 * @brief Returns the local vertex identifier in network nid corresponding to global identity gvid.
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	vertex_id getVertexId(global_identity gvid, network_id nid) const;
	/**
	 * @brief Returns the local vertex name in network network_name corresponding to global vertex global_vertex_name.
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	std::string getVertexName(const std::string& global_identity, const std::string& network_name) const;
	/**
	 * @brief Finds the global identity corresponding to vertex lvid in local network nid.
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	global_identity getGlobalIdentity(vertex_id lvid, network_id nid) const;
	/**
	 * @brief Finds the global identifier corresponding to vertex local_vertex_name in local network network_name.
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	std::string getGlobalName(const std::string& local_vertex_name, const std::string& network_name) const;
	/**
	 * @brief Checks if there is a global vertex with identifier gvid.
	 **/
	bool containsGlobalIdentity(global_identity gvid) const;
	/**
	 * @brief Checks if there is a global vertex global_vertex_name.
	 **/
	bool containsGlobalName(const std::string& global_identity) const;
	/**
	 * @brief Checks if global vertex gvid is associated to a vertex in network nid.
	 **/
	bool containsVertex(global_identity gvid, network_id nid) const;
	/**
	 * @brief Checks if global vertex global_vertex_name is associated to a vertex in network network_name.
	 **/
	bool containsVertex(const std::string& global_identity_name, const std::string& network_name) const;
private:
	// from a vertex to its global identity
	std::map<global_vertex_id, global_identity> local_to_global_id;
	// from global_identity to the corresponding vertex_id on network_id
	std::vector<std::map<network_id, vertex_id> > global_to_local_id;
	/* Conversion from numerical to string ids */
	std::map<global_identity, std::string> identity_id_to_name;
	std::map<std::string, global_identity> identity_name_to_id;

};

void print(Multiplex& mnet);

/**********************************************************************/
/** Multiple Network *************************************************/
/**********************************************************************/
class MultipleNetwork : public MultilayerNetwork {
public:
	/** Creates an empty network (with 0 internal networks) */
	MultipleNetwork();
	/** */
	~MultipleNetwork();
	/**
	 * @brief Creates a link between two vertexes on different networks.
	 * All interlayer connections are directed (TODO is this OK?)
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 * @throws OperationNotSupportedException if the two vertexes lay on the same network
	 **/
	void newInterlayerConnection(global_vertex_id v1, global_vertex_id v2);
	/**
	 * @brief Inserts into "connections" all the edge identifiers of interlayer connections.
	 **/
	void getInterlayerConnections(std::set<interlayer_edge_id>& connections) const;

};

void print(MultipleNetwork& mnet);

/* Path class to be commented */

class Path {
private:
	const MultilayerNetwork *mnet;
	std::vector<long> num_edges_per_layer;
	std::vector<vertex_id> path;
	std::vector<network_id> network;
	long timestamp;

	friend std::ostream& operator<<(std::ostream &strm, const Path &path);

public:
	Path(const Path& p, long timestamp);
	Path(MultilayerNetwork& mnet, long timestamp);
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



#endif /* MULTIPLENETWORK_DATASTRUCTURES_H_ */
