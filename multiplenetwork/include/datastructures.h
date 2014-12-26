/**
 * datastructures.h
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 1.0
 *
 * This file defines the basic data structures of the library.
 * 
 * The library is used to analyze connected entities ("entity_id"), that are also called actors
 * when they represent individuals.
 *
 * Entities can belong to multiple networks. Every network contains vertexes and edges.
 * "vertex_id" and "edge_id" uniquely identify vertexes and edges inside a specific network,
 * while "global_vertex_id" and "global_edge_id" globally identify vertexes and edges (that is,
 * they also include an identifier of the network - "network_id"). Each vertex corresponds to a
 * global entity ("entity_id"). In this way, for example vertexes in different social networks
 * can refer to the same actor, as it happens when an individual owns multiple accounts on mutiple
 * online social network sites. In addition to implicit references between vertexes (through references
 * to the same global entity), edges connecting vertexes on different networks are also possible.
 *
 * The main classes defined in this file are Network and MultipleNetwork.
 *
 * A class representing a path between two vertexes, where the path can traverse multiple networks,
 * is also defined here and it is used to compute distances between entities.
 *
 * BASIC TYPES and CLASSES:
 * entity_id
 * network_id, vertex_id, edge_id,
 * global_vertex_id, global_edge_id,
 * Network, MultipleNetwork, Path
 */

#ifndef MULTIPLENETWORK_DATASTRUCTURES_H_
#define MULTIPLENETWORK_DATASTRUCTURES_H_

#include <string>
#include <map>
#include <set>
#include <vector>

/**
 * The identifier of a global entity, also called actor when it represents an individual.
 * Implicit connections between vertexes in different networks can be defined by referring
 * to the same global entity.
 */
typedef long identity;

/** The identifier of a network inside a multiple network */
typedef int network_id;

/** The identifier of a vertex inside a single network */
typedef long vertex_id;

/**
 * The identifier of an edge inside a single network.
 * An edge is identified by the two vertexes it connects and
 * its directionality. Directionality is used to compute
 * the equality operator: if an edge is undirected, (u,v)==(v,u)
 * (i.e., the two edge_ids refer to the same edge: they are equal)
 */
class edge_id {
public:
	vertex_id v1;
	vertex_id v2;
	bool directed;
	edge_id(vertex_id v1, vertex_id v2, bool directed);
	bool operator==(const edge_id& e2) const;
	bool operator!=(const edge_id& e2) const;
	bool operator<(const edge_id& e2) const;
	bool operator>(const edge_id& e2) const;
	friend std::ostream& operator<<(std::ostream &strm, const edge_id& eid);
	std::string to_string() const;
};

/**
 * The global identifier of a vertex inside a multiple network.
 */
class global_vertex_id {
public:
	vertex_id vid;
	network_id network;
	global_vertex_id(vertex_id vid, network_id network);
	bool operator==(const global_vertex_id& e2) const;
	bool operator!=(const global_vertex_id& e2) const;
	bool operator<(const global_vertex_id& e2) const;
	bool operator>(const global_vertex_id& e2) const;
	friend std::ostream& operator<<(std::ostream &strm, const global_vertex_id& eid);
	std::string to_string() const;
};

/**
 * The identifier of an edge between two vertexes in a multiple network.
 * An edge is identified by the two nodes it connects, the network and
 * its directionality. Directionality is used to evaluate
 * the equality operator: if an edge is undirected, (u,v)==(v,u)
 * (i.e., the two edge_ids refer to the same edge: they are equal).
 */
class global_edge_id {
public:
	vertex_id v1;
	vertex_id v2;
	network_id network;
	bool directed;
	global_edge_id(vertex_id v1, vertex_id v2, network_id network, bool directed);
	bool operator==(const global_edge_id& e2) const;
	bool operator!=(const global_edge_id& e2) const;
	bool operator<(const global_edge_id& e2) const;
	bool operator>(const global_edge_id& e2) const;
	friend std::ostream& operator<<(std::ostream &strm, const global_edge_id& eid);
	std::string to_string() const;
};

/**
 * The identifier of an edge between two vertexes on two (potentially different) networks.
 * An edge is identified by the two global vertexes it connects and
 * its directionality. Directionality is used to compute
 * the equality operator: if an edge is undirected, (u,n1,v,n2)==(v,n2,u,n1)
 * (i.e., the two edge_ids refer to the same edge: they are equal).
 * For example, Matteo on the "Supervisors" friendship network and
 * Luca on the "PhD Students" friendship network can be connected by an
 * edge indicating that Matteo supervises Luca).
 */
class inter_edge_id {
public:
	global_vertex_id v1;
	global_vertex_id v2;
	bool directed;
	inter_edge_id(global_vertex_id v1, global_vertex_id v2, bool directed);
	bool operator==(const inter_edge_id& e2) const;
	bool operator!=(const inter_edge_id& e2) const;
	bool operator<(const inter_edge_id& e2) const;
	bool operator>(const inter_edge_id& e2) const;
	friend std::ostream& operator<<(std::ostream &strm, const inter_edge_id& eid);
	std::string to_string() const;
};

/**********************************************************************/
/** Network ***********************************************************/
/**********************************************************************/
class Network {
public:
	/****************************/
	/* Constructors/destructors */
	/****************************/
	/** Creates an unnamed, undirected and unweighted empty network */
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
	 * @return the id of the new vertex
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
	 **/
	bool containsVertex(const std::string& vertex_name) const;
	/**
	 * @brief Returns true if the edge is present.
	 * In an undirected network an edge a-b is returned also if it was inserted as b-a.
	 **/
	bool containsEdge(vertex_id vid1, vertex_id vid2) const;
	/**
	 * @brief Returns true if the vertex is present.
	 * @throws OperationNotSupportedException if the network is not named
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
	bool isWeighted() const;
	/**
	 * @brief Returns true if the network is named, i.e., vertexes are identified by their names
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
	 * Used to efficiently iterate over all the vertexes
	 **/
	const std::set<vertex_id>& getVertexes() const;
	/**
	 * @brief Returns all the edge identifiers in the network.
	 * This method is normally used to iterate over all the edges
	 * @param edges set where the edge ids are inserted
	 **/
	const std::set<edge_id>& getEdges() const;
	/**
	 * @brief Returns the number of vertexes.
	 **/
	long getNumVertexes() const;
	/**
	 * @brief Returns the number of edges.
	 * In an undirected network an edge a-b is counted only once (not twice by also considering b-a).
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
	std::set<vertex_id> getOutNeighbors(vertex_id vid) const;
	/**
	 * @brief Returns the identifiers of the vertexes with an edge towards vid.
	 **/
	std::set<vertex_id> getInNeighbors(vertex_id vid) const;
	/**
	 * @brief Returns the identifiers of the vertexes with an edge from or towards to vid.
	 **/
	std::set<vertex_id> getNeighbors(vertex_id vid) const;
	/**
	 * @brief Returns the names of the vertexes with an edge coming from vertex_name.
	 * Notice that vertexes are stored by id, therefore the unnamed version of this function is faster.
	 * @throws ElementNotFoundException if the input vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	std::set<std::string> getOutNeighbors(const std::string& vertex_name) const;
	/**
	 * @brief Returns the name of the vertexes with an edge going to vertex_name.
	 * Notice that vertexes are stored by id, therefore the unnamed version of this function is faster.
	 * @throws ElementNotFoundException if the input vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	std::set<std::string> getInNeighbors(const std::string& vertex_name) const;
	/**
	 * @brief Returns the name of the vertexes with an edge coming from or going to vertex_name.
	 * Notice that vertexes are stored by id, therefore the unnamed version of this function is faster.
	 * @throws ElementNotFoundException if the input vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	std::set<std::string> getNeighbors(const std::string& vertex_name) const;
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
	 * @brief Checks if a vertex attribute with this name already exists.
	 * @param attribute_name The name of the vertex attribute
	 **/
	bool hasVertexAttribute(const std::string& attribute_name) const;
	/**
	 * @brief Checks if a string vertex attribute with this name already exists.
	 * @param attribute_name The name of the vertex attribute
	 **/
	bool hasStringVertexAttribute(const std::string& attribute_name) const;
	/**
	 * @brief Checks if a numeric vertex attribute with this name already exists.
	 * @param attribute_name The name of the vertex attribute
	 **/
	bool hasNumericVertexAttribute(const std::string& attribute_name) const;
	/**
	 * @brief Checks if an edge attribute with this name already exists.
	 * @param attribute_name The name of the vertex attribute
	 **/
	bool hasEdgeAttribute(const std::string& attribute_name) const;
	/**
	 * @brief Checks if a string edge attribute with this name already exists.
	 * @param attribute_name The name of the vertex attribute
	 **/
	bool hasStringEdgeAttribute(const std::string& attribute_name) const;
	/**
	 * @brief Checks if a numeric edge attribute with this name already exists.
	 * @param attribute_name The name of the vertex attribute
	 **/
	bool hasNumericEdgeAttribute(const std::string& attribute_name) const;
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
	/**
	 * @brief Returns the names of the numeric edge attributes defined for this network.
	 **/
	std::set<std::string> getNumericEdgeAttributes() const;
	/**
	 * @brief Returns the names of the string edge attributes defined for this network.
	 **/
	std::set<std::string> getStringEdgeAttributes() const;
	/**
	 * @brief Returns the names of the numeric edge attributes defined for this network.
	 **/
	std::set<std::string> getNumericVertexAttributes() const;
	/**
	 * @brief Returns the names of the string edge attributes defined for this network.
	 **/
	std::set<std::string> getStringVertexAttributes() const;
	/**
	 * @brief Returns the number of string vertex attributes defined for this network.
	 **/
	int getNumStringVertexAttributes() const;
	/**
	 * @brief Returns the number of numeric vertex attributes defined for this network.
	 **/
	int getNumNumericVertexAttributes() const;
	/**
	 * @brief Returns the number of string edge attributes defined for this network.
	 **/
	int getNumStringEdgeAttributes() const;
	/**
	 * @brief Returns the number of numeric edge attributes defined for this network.
	 **/
	int getNumNumericEdgeAttributes() const;
	/**
	 * @brief Returns the number of attributes defined for this network.
	 **/
	int getNumAttributes() const;

	friend std::ostream& operator<<(std::ostream &strm, const Network& net);

private:
	// largest vertex identifier assigned so far
	vertex_id max_vertex_id;
	long num_edges;
	bool is_named, is_directed, is_weighed;
	// Set of vertex ids for all vertexes in the network, used for both named and unnamed networks. (redundant in case of named networks)
	std::set<vertex_id> vertexes;
	// Set of edge ids for all edges in the network
	std::set<edge_id> edges;
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

/**********************************************************************/
/** Multiple Network ************************************************/
/**********************************************************************/
class MultipleNetwork {
public:
	/** Creates an empty network (with 0 internal networks) */
	MultipleNetwork();
	/** */
	~MultipleNetwork();
	/**
	 * @brief Adds a local network to this multiplenetwork data structure.
	 * A default name is automatically associated to the new network
	 * @return the identifier of the newly added network
	 **/
	network_id addNetwork(const Network& net);
	/**
	 * @brief Adds a named local network to this multiplenetwork data structure.
	 * @return the identifier of the newly added network
	 **/
	network_id addNetwork(const std::string& network_name, Network& net);
	/**
	 * @brief Returns into "networks" all the network identifiers.
	 **/
	std::set<network_id> getNetworks() const;
	/**
	 * @brief Returns all the network names.
	 **/
	std::set<std::string> getNetworkNames() const;
	/**
	 * @brief Returns all the (global) vertex identifiers.
	 **/
	std::set<global_vertex_id> getVertexes() const;
	/**
	 * @brief Inserts into "edges" all the (global) edge identifiers.
	 **/
	std::set<global_edge_id> getEdges() const;
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
	 * @brief Returns a pointer to the network network_name (const version).
	 **/
	const Network& getNetwork(const std::string& network_name) const;
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

	friend std::ostream& operator<<(std::ostream &strm, const MultipleNetwork& net);

private:
	// The networks composing this multiple network system
	std::vector<Network> networks;
	// Conversion from symbolic names to numerical ids and back
	std::map<std::string, network_id> network_name_to_id;
	std::vector<std::string> network_id_to_name;
};

/**********************************************************************/
/** Multiplex Network *************************************************/
/**********************************************************************/
class MultiplexNetwork : public MultipleNetwork {
public:
	/** Creates an empty network (with 0 internal networks) */
	MultiplexNetwork();
	/** */
	~MultiplexNetwork();
	/**
	 * @brief Adds a global vertex to the network.
	 * Global vertexes are global identifiers used to relate vertexes in the local networks.
	 * For example, a local node "1" in network "0" and a local node "13" in network "1" may refer
	 * to the same global vertex.
	 * @return the vertex identifier of the new global vertex
	 **/
	entity_id addGlobalIdentity();
	/**
	 * @brief Adds a named global vertex to the network.
	 * Global vertexes are global identifiers used to relate vertexes in the local networks.
	 * For example, a local node "v1" in network "v0" and a local node "v13" in network "v1" may refer
	 * to the same global vertex.
	 * @return the vertex identifier of the new global vertex
	 **/
	entity_id addGlobalName(const std::string& name);
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
	void map(identity gvid, vertex_id lvid, network_id nid);
	/**
	 * @brief Defines that vertex local_vertex_name in network network_name corresponds to global vertex global_vertex_name.
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 **/
	void mapIdentity(const std::string& global_vertex_name, const std::string& local_vertex_name, const std::string& network_name);
	/**
	 * @brief returns all the global identities.
	 **/
	std::set<identity> getGlobalIdentities() const;
	/**
	 * @brief returns all the global names.
	 **/
	std::set<std::string> getGlobalNames() const;
	/**
	 * @brief Returns the name of vertex vertex_id.
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	std::string getGlobalName(identity gid) const;
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
	vertex_id getVertexId(identity gvid, network_id nid) const;
	/**
	 * @brief Returns the local vertex name in network network_name corresponding to global vertex global_vertex_name.
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	std::string getVertexName(const std::string& global_identity, const std::string& network_name) const;
	/**
	 * @brief Finds the global identity corresponding to vertex lvid in local network nid.
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	entity_id getGlobalIdentity(vertex_id lvid, network_id nid) const;
	/**
	 * @brief Finds the global identifier corresponding to vertex local_vertex_name in local network network_name.
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	std::string getGlobalName(const std::string& local_vertex_name, const std::string& network_name) const;
	/**
	 * @brief Checks if there is a global vertex with identifier gvid.
	 **/
	bool containsGlobalIdentity(identity gvid) const;
	/**
	 * @brief Checks if there is a global vertex global_vertex_name.
	 **/
	bool containsGlobalName(const std::string& global_identity) const;
	/**
	 * @brief Checks if global vertex gvid is associated to a vertex in network nid.
	 **/
	bool containsVertex(identity gvid, network_id nid) const;
	/**
	 * @brief Checks if global vertex global_vertex_name is associated to a vertex in network network_name.
	 **/
	bool containsVertex(const std::string& global_identity_name, const std::string& network_name) const;

	// Attribute management
	bool hasAttribute(const std::string& attribute_name) const;
	bool hasNumericAttribute(const std::string& attribute_name) const;
	bool hasStringAttribute(const std::string& attribute_name) const;
	void newStringAttribute(const std::string& attribute_name);
	std::string getStringAttribute(const identity& global_id, const std::string& attribute_name) const;
	void setStringAttribute(const identity& global_id, const std::string& attribute_name, const std::string& value);
	std::string getStringAttribute(const std::string& global_name, const std::string& attribute_name) const;
	void setStringAttribute(const std::string& global_name, const std::string& attribute_name, const std::string& value);
	void newNumericAttribute(const std::string& attribute_name);
	double getNumericAttribute(const identity& global_id, const std::string& attribute_name) const;
	void setNumericAttribute(const identity& global_id, const std::string& attribute_name, double value);
	double getNumericAttribute(const std::string& global_name, const std::string& attribute_name) const;
	void setNumericAttribute(const std::string& global_name, const std::string& attribute_name, double value);
	std::set<std::string> getNumericAttributes() const;
	std::set<std::string> getStringAttributes() const;

	bool deleteVertex(identity vid, network_id network);
	bool deleteVertex(const std::string& global_name, const std::string& network);

	friend std::ostream& operator<<(std::ostream &strm, const MultiplexNetwork& net);

private:
	// from a vertex to its global identity
	std::map<global_vertex_id, identity> local_to_global_id;
	// from global_identity to the corresponding vertex_id on network_id
	std::vector<std::map<network_id, vertex_id> > global_to_local_id;
	/* Conversion from numerical to string ids */
	std::map<identity, std::string> identity_id_to_name;
	std::map<std::string, identity> identity_name_to_id;
	// Global IDs attributes
	std::map<std::string, std::map<identity, std::string> > string_attribute;
	std::map<std::string, std::map<identity, double> > numeric_attribute;
};

/**********************************************************************/
/** Multiple Network *************************************************/
/**********************************************************************/
class InterdependentNetwork : public InterdependentNetwork {
public:
	/** Creates an empty network (with 0 internal networks) */
	InterdependentNetwork();
	/** */
	~InterdependentNetwork();
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
	void getInterlayerConnections(std::set<inter_edge_id>& connections) const;

	friend std::ostream& operator<<(std::ostream &strm, const MultipleNetwork& net);
};

/**********************************************************************/
/** Distance **************************************************************/
/**********************************************************************/

class Distance {
private:
	const MultipleNetwork *mnet;
	std::vector<long> num_edges_per_layer;
	long timestamp;
	long num_steps;

	friend std::ostream& operator<<(std::ostream &strm, const Distance &dist);

public:
	/**
	 * @brief Constructs a
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 * @throws OperationNotSupportedException if the two vertexes lay on the same network
	 **/
	Distance(const Distance& p, long timestamp);
	Distance(const MultipleNetwork& mnet, long timestamp);
	Distance(long num_layers, long timestamp);
	virtual ~Distance();

	/**
	 * @brief Creates a link between two vertexes on different networks.
	 * All interlayer connections are directed (TODO is this OK?)
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 * @throws OperationNotSupportedException if the two vertexes lay on the same network
	 **/
	long getNumNetworks() const;

	long getTimestamp() const;

	long getNumEdgesOnNetwork(long layer) const;

	void extend(network_id nid);

	bool operator<(const Distance& other) const;

	Distance operator=(const Distance& other) const;

	bool same(const Distance& other) const;

	long length() const;
};


/**********************************************************************/
/** Path **************************************************************/
/**********************************************************************/

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
	Path(const MultipleNetwork& mnet, long timestamp);
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
