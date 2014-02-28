/*
 * datastructures.h
 *
 * Created on: Feb 6, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 *
 * CLASSES: Network
 */

#ifndef MULTIPLENETWORK_DATASTRUCTURES_H_
#define MULTIPLENETWORK_DATASTRUCTURES_H_

#include "types.h"
#include <string>
#include <map>
#include <set>
#include <vector>

struct edge {
public:
	vertex_id v1;
	vertex_id v2;
	network_id network;

	edge(vertex_id v1, vertex_id v2, network_id network);

	bool operator==(const edge& e2) const;

	bool operator<=(const edge& e2) const;

	bool operator<(const edge& e2) const;
};

/**********************************************************************/
/** Network ***********************************************************/
/**********************************************************************/
class Network {
public:
	/****************************/
	/* Constructors/destructors */
	/****************************/
	/** Not to be used */
	Network();
	/** Creates an empty network */
	Network(bool named, bool directed, bool weighted);
	/** */
	~Network();
	/*******************************/
	/* Basic structural operations */
	/*******************************/
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
	vertex_id addVertex(std::string vertex_name);
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
	edge_id addEdge(std::string vertex_name1, std::string vertex_name2);
	/**
	 * @brief Adds a new edge with an associated double precision weight.
	 * The type of edge (directed/undirected) is inherited by the type of network (specified at creation time).
	 * @param vertex_name1 the "from" vertex in a directed network, or one end of the edge in an undirected one
	 * @param vertex_name2 the "to" vertex in a directed network, or one end of the edge in an undirected one
	 * @param weight
	 * @throws OperationNotSupportedException if the network is not weighted
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 * @throws DuplicateElementException if the edge is already present in the network
	 **/
	edge_id addEdge(std::string vertex_name1, std::string vertex_name2,
			double weight);
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
	 * @throws OperationNotSupportedException if the network is unnamed
	 * @return false if the vertex is not present in the network
	 **/
	bool deleteVertex(std::string vertex_name);
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
	 **/
	bool deleteEdge(std::string vertex_name1, std::string vertex_name2);
	/***********************/
	/* Getters and setters */
	/***********************/
	void getVertexes(std::set<vertex_id>& vertexes);
	/**
	 * @brief Returns the number of vertexes.
	 **/
	long getNumVertexes();
	/**
	 * @brief Returns the number of edges.
	 * In an undirected networks an edge a-b is counted only once (not twice by also considering b-a).
	 **/
	long getNumEdges();
	/**
	 * @brief Returns the number of outgoing edges from vertex vid.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getOutDegree(vertex_id vid);
	/**
	 * @brief Returns the number of incoming edges from vertex vid.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getInDegree(vertex_id vid);
	/**
	 * @brief Returns the number of incoming or outgoing edges from/to vertex vid.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getDegree(vertex_id vid);
	/**
	 * @brief Returns the number of outgoing edges from vertex name.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getOutDegree(std::string vertex_name);
	/**
	 * @brief Returns the number of incoming edges from vertex name.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getInDegree(std::string vertex_name);
	/**
	 * @brief Returns the number of incoming or outgoing edges from/to vertex name.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	long getDegree(std::string vertex_name);
	/**
	 * @brief Returns the identifiers of the vertexes with an edge coming from vertex_id.
	 * @throws ElementNotFoundException if the input vertex id is not present in the network
	 **/
	void getOutNeighbors(vertex_id vid, std::set<vertex_id>& neighbors);
	/**
	 * @brief Returns the identifiers of the vertexes with an edge going to vertex_id.
	 **/
	void getInNeighbors(vertex_id vid, std::set<vertex_id>& neighbors);
	/**
	 * @brief Returns the identifiers of the vertexes with an edge coming from or going to vertex_id.
	 **/
	void getNeighbors(vertex_id vid, std::set<vertex_id>& neighbors);
	/**
	 * @brief Returns the names of the vertexes with an edge coming from vertex_name.
	 * Notice that vertexes are stored by id, therefore the unnamed version of this function is faster.
	 * @throws ElementNotFoundException if the input vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	void getOutNeighbors(std::string vertex_name, std::set<std::string>& neighbors);
	/**
	 * @brief Returns the name of the vertexes with an edge going to vertex_name.
	 * Notice that vertexes are stored by id, therefore the unnamed version of this function is faster.
	 * @throws ElementNotFoundException if the input vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	void getInNeighbors(std::string vertex_name, std::set<std::string>& neighbors);
	/**
	 * @brief Returns the name of the vertexes with an edge coming from or going to vertex_name.
	 * Notice that vertexes are stored by id, therefore the unnamed version of this function is faster.
	 * @throws ElementNotFoundException if the input vertex is not present in the network
	 * @throws OperationNotSupportedException if the network is unnamed
	 **/
	void getNeighbors(std::string vertex_name, std::set<std::string>& neighbors);
	/**
	 * @brief Returns the weight on this edge.
	 * @throws OperationNotSupportedException if the network is not weighted
	 * @throws ElementNotFoundException if the input edge is not present in the network
	 **/
	double getEdgeWeight(vertex_id vid1, vertex_id vid2);
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
	double getEdgeWeight(std::string vertex_name1, std::string vertex_name2);
	/**
	 * @brief Sets the weight of edge eid.
	 * @throws OperationNotSupportedException if the network is not weighted or not named
	 * @throws ElementNotFoundException if the input edge is not present in the network
	 **/
	void setEdgeWeight(std::string vertex_name1, std::string vertex_name2,
			double weight);
	/**
	 * @brief Returns the name of vertex vertex_id.
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	std::string getVertexName(vertex_id vid);
	/**
	 * @brief Returns the id of the vertex with name vertex_name.
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	vertex_id getVertexId(std::string vertex_name);


	/****************************/
	/* Vertex and edge sets      */
	/****************************/
	std::set<vertex_id> getVertexes();
	/****************************/
	/* Check network properties */
	/****************************/
	/**
	 * @brief Returns true if the network is directed
	 */
	bool isDirected();
	/**
	 * @brief Returns true if the network is weighted
	 */
	bool isWeighed();
	/**
	 * @brief Returns true if the network is named
	 */
	bool isNamed();
	/**
	 * @brief Returns true if the vertex is present.
	 **/
	bool containsVertex(vertex_id vid);
	/**
	 * @brief Returns true if the vertex is present.
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if the input element is not present in the network
	 **/
	bool containsVertex(std::string vertex_name);
	/**
	 * @brief Returns true if the edge is present.
	 * In an undirected network an edge a-b is returned also if it was inserted as b-a.
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 **/
	bool containsEdge(vertex_id vid1, vertex_id vid2);
	/**
	 * @brief Returns true if the vertex is present.
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 **/
	bool containsEdge(std::string vertex_name1, std::string vertex_name2);
	/**********************/
	/* Attribute handling */
	/**********************/
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
	std::string getStringVertexAttribute(vertex_id vid,
			std::string attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	std::string getStringVertexAttribute(std::string vertex_name,
			std::string attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	void setStringVertexAttribute(vertex_id vid, std::string attribute_name,
			std::string value);
	/**
	 * @brief Sets the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is set
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	void setStringVertexAttribute(std::string vertex_name,
			std::string attribute_name, std::string value);
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
	double getNumericVertexAttribute(vertex_id vid, std::string attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	double getNumericVertexAttribute(std::string vertex_name,
			std::string attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	void setNumericVertexAttribute(vertex_id vid, std::string attribute_name,
			double value);
	/**
	 * @brief Sets the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is set
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	void setNumericVertexAttribute(std::string vertex_name,
			std::string attribute_name, double value);
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
	std::string getStringEdgeAttribute(vertex_id vid1, vertex_id vid2,
			std::string attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	std::string getStringEdgeAttribute(std::string vertex_name1,
			std::string vertex_name2, std::string attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	void setStringEdgeAttribute(vertex_id vid1, vertex_id vid2,
			std::string attribute_name, std::string value);
	/**
	 * @brief Sets the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is set
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	void setStringEdgeAttribute(std::string vertex_name1,
			std::string vertex_name2, std::string attribute_name,
			std::string value);
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
			std::string attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	double getNumericEdgeAttribute(std::string vertex_name1,
			std::string vertex_name2, std::string attribute_name);
	/**
	 * @brief Retrieves the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is retrieved
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws ElementNotFoundException if there is no vertex with this id
	 **/
	void setNumericEdgeAttribute(vertex_id vid1, vertex_id vid2,
			std::string attribute_name, double value);
	/**
	 * @brief Sets the value of a vertex attribute.
	 * @param vertex_id the id of the vertex whose associated value is set
	 * @param attribute_name The name of the vertex attribute
	 * @param value The value to be set
	 * @throws OperationNotSupportedException if the network is not named
	 * @throws ElementNotFoundException if there is no vertex with this name
	 **/
	void setNumericEdgeAttribute(std::string vertex_name1,
			std::string vertex_name2, std::string attribute_name, double value);
private:
	long max_vertex_id;
	long max_edge_id;
	long num_edges;
	bool is_named, is_directed, is_weighed;
	/* (numeric) vertexes and edges */
	/**
	 * Used for both named and unnamed networks. (redundant in case of named networks, but allows named functions to be implemented in a simpler way by getting the vertex ids and calling the unnamed functions)
	 */
	std::set<vertex_id> vertexes;
	/**
	 * Used for named networks.
	 */
	std::map<vertex_id, std::string> vertex_id_to_name;
	std::map<std::string, vertex_id> vertex_name_to_id;
	std::map<vertex_id, std::map<vertex_id, edge_id> > out_edges;
	std::map<vertex_id, std::map<vertex_id, edge_id> > in_edges;
	/* attributes */
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
	 *
	 * Global vertexes are global identifiers used to relate vertexes in the local networks.
	 * For example, a local node "1" in network "0" and a local node "13" in network 1 may refer
	 * to the same global vertex.
	 * This operation is available as soon as a network has been created.
	 **/
	vertex_id addVertex();
	/**
	 * @brief Adds num_new_vertexes global vertexes to the network.
	 *
	 * Global vertexes are global identifiers used to relate vertexes in the local networks.
	 * For example, a local node "1" in network "0" and a local node "13" in network 1 may refer
	 * to the same global vertex.
	 * This operation is available as soon as a network has been created.
	 *
	 * When global identities have been created they cannot be deleted. However, a global vertex may end up
	 * having no corresponding local vertexes in any local network.
	 **/
	void addVertexes(long num_new_vertexes);
	/**
	 * @brief Adds a (directed or indirected) local network to this multiplenetwork data structure.
	 *
	 * Only after a local network has been added, nodes and edges can be created.
	 **/
	network_id addNetwork(Network& net);
	/**
	 * @brief
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 **/
	void map(vertex_id global_vertex_id, vertex_id local_vertex_id, network_id nid);

	/****************************/
	/* Vertex and edge set      */
	/****************************/
	void getNetworks(std::set<network_id>& networks);
	void getVertexes(std::set<vertex_id>& vertexes);
	void getEdges(std::set<edge>& edges);
	/**
	 * @brief Finds the identifier of a global vertex inside a local network.
	 *
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	long getLocalVertexId(vertex_id global_vertex_id, int nid);
	/**
	 * @brief Finds the global identifier of a vertex in a local network.
	 *
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	long getGlobalVertexId(vertex_id local_vertex_id, int nid);
	/**
	 * @brief Returns the number of local networks.
	 **/
	int getNumNetworks();
	/**
	 * @brief Returns the number of global vertexes.
	 **/
	long getNumVertexes();
	/**
	 * @brief Returns the number of edges.
	 * In an undirected networks an edge a-b is counted only once (not twice by also considering b-a). This can create confusion
	 * when some local networks are directed and some are undirected.
	 **/
	long getNumEdges();

	Network* getNetwork(network_id nid);

	bool containsVertex(vertex_id global_vertex_id);
	bool containsNetwork(network_id nid);
	bool containsVertex(vertex_id global_vertex_id, network_id nid);
	/*
	bool containsGlobalEdge(vertex_id global_vertex_id1,
			vertex_id global_vertex_id2, network_id nid);
	 */

	// basic conversion functions
	vertex_id addVertex(std::string name);
	network_id addNetwork(std::string network_name, Network& net);
	/**
	 * @brief
	 * @throws ElementNotFoundException if the input elements are not present in the network
	 **/
	void map(std::string global_vertex_name1, std::string global_vertex_name2, std::string network_name);

	/**
	 * @brief Finds the identifier of a global vertex inside a local network.
	 *
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	std::string getLocalVertexName(std::string global_vertex_name, std::string network_name);
	/**
	 * @brief Finds the global identifier of a vertex in a local network.
	 *
	 * @throws ElementNotFoundException if the vertex of network is not present
	 **/
	std::string getGlobalVertexName(std::string local_vertex_name, std::string network_name);

	bool containsVertex(std::string global_vertex_name);

	bool containsVertex(std::string global_vertex_name, std::string network_name);
	bool containsNetwork(std::string network_name);
	std::string getNetworkName(network_id nid);
	int getNetworkId(std::string network_name);

	std::string getVertexName(vertex_id global_vertex_id);
	long getVertexId(std::string global_vertex_name);

	Network* getNetwork(std::string network_name);

private:
	std::vector<Network> graphs;

	// how to use: local_to_global_id[nid][local_vertex_id]
	std::vector<std::map<vertex_id, vertex_id> > local_to_global_id;
	// how to use: global_to_local_id[global_vertex_id][nid]
	std::vector<std::map<network_id, vertex_id> > global_to_local_id;

	// conversion from symbolic names to numerical ids and back

	std::map<std::string, network_id> network_name_to_id;
	std::vector<std::string> network_id_to_name;

	std::map<std::string, vertex_id> vertex_name_to_id;
	std::vector<std::string> vertex_id_to_name;

};

void print(MultipleNetwork& mnet);

class Path {
private:
	const MultipleNetwork *mnet;
	std::vector<long> num_edges_per_layer;
	std::vector<vertex_id> path;
	// network[i] contains the identifier of the network where the edge path[i],path[i+1] is;
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

class Distance {
public:
	Distance();
	virtual ~Distance();
};

#endif /* MULTIPLENETWORK_DATASTRUCTURES_H_ */
