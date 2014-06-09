/* Network.cpp
 *
 * All comments explaining the effect of these functions and not
 * related to specific implementation choices are in datastructures.h
 */

#include "datastructures.h"
#include "exceptions.h"
#include "utils.h"
#include <iostream>

Network::Network() {
	Network(false,false,false);
}

Network::Network(bool named, bool directed, bool weighed) {
	max_vertex_id=-1;
	num_edges=0;
	is_named=named;
	is_weighed=weighed;
	is_directed=directed;
	// weights are stored in the same way as any other edge attribute
	if (is_weighed) {
		newNumericEdgeAttribute("weight");
	}
}

Network::~Network() {
	// TODO
}

vertex_id Network::addVertex() {
	if (isNamed()) throw OperationNotSupportedException("Cannot add an anonymous vertex to a named network");
	max_vertex_id++;
	vertex_id new_vertex_id = max_vertex_id;
	vertexes.insert(new_vertex_id);
	return new_vertex_id;
}

vertex_id Network::addVertex(const std::string& vertex_name) {
	if (containsVertex(vertex_name)) throw DuplicateElementException("Vertex " + vertex_name + " already exists");
	if (!isNamed()) throw OperationNotSupportedException("Cannot add a named vertex to an unnamed network");
	max_vertex_id++;
	vertex_id new_vertex_id = max_vertex_id;
	vertexes.insert(new_vertex_id);
	vertex_id_to_name[new_vertex_id] = vertex_name;
	vertex_name_to_id[vertex_name] = new_vertex_id;
	return new_vertex_id;
}

edge_id Network::addEdge(vertex_id vid1, vertex_id vid2) {
	if (!containsVertex(vid1)) throw ElementNotFoundException("Vertex " + std::to_string(vid1));
	if (!containsVertex(vid2)) throw ElementNotFoundException("Vertex " + std::to_string(vid2));
	if (out_edges[vid1].count(vid2)>0) throw DuplicateElementException("Edge (" + std::to_string(vid1) + "," + std::to_string(vid2) +  ") already exists");
	//max_edge_id++;
	out_edges[vid1].insert(vid2);
	in_edges[vid2].insert(vid1);
	if (!isDirected() && vid1!=vid2) {
		out_edges[vid2].insert(vid1);
		in_edges[vid1].insert(vid2);
	}
	if (isWeighed()) setNumericEdgeAttribute(vid1,vid2,"weight",MULTIPLENETWORK_DEFAULT_WEIGHT); // this value will be replaced if this method has been called inside addEdge(vertex_id, vertex_id, double)
	num_edges++;
	return edge_id(vid1, vid2, isDirected());
}

edge_id Network::addEdge(vertex_id vid1, vertex_id vid2, double weight) {
	if (!isWeighed()) throw OperationNotSupportedException("Cannot add a weight: network is unweighed");
	addEdge(vid1,vid2);
	setNumericEdgeAttribute(vid1, vid2, "weight", weight);
	return edge_id(vid1, vid2, isDirected());
}

edge_id Network::addEdge(const std::string& vertex_name1, const std::string& vertex_name2) {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name1)) throw ElementNotFoundException("Vertex " + vertex_name1);
	if (!containsVertex(vertex_name2)) throw ElementNotFoundException("Vertex " + vertex_name2);
	return addEdge(vertex_name_to_id[vertex_name1],vertex_name_to_id[vertex_name2]);
}

edge_id Network::addEdge(const std::string& vertex_name1, const std::string& vertex_name2, double weight) {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name1)) throw ElementNotFoundException("Vertex " + vertex_name1);
	if (!containsVertex(vertex_name2)) throw ElementNotFoundException("Vertex " + vertex_name2);
	return addEdge(vertex_name_to_id[vertex_name1],vertex_name_to_id[vertex_name2], weight);
}

bool Network::deleteVertex(vertex_id vid) {
	if (!containsVertex(vid)) return false;
	std::set<vertex_id> in = getInNeighbors(vid);
	std::set<vertex_id> out = getOutNeighbors(vid);

	// removing adjacent edges
	for (std::set<vertex_id>::iterator it=in.begin(); it!=in.end(); it++)
		deleteEdge(*it,vid);
	for (std::set<vertex_id>::iterator it=out.begin(); it!=out.end(); it++)
		deleteEdge(vid,*it);

	// removing the vertex
	vertexes.erase(vid);
	if (isNamed()) {
		// some more structures to empty
		const std::string& vertex_name=vertex_id_to_name[vid];
		vertex_id_to_name.erase(vid);
		vertex_name_to_id.erase(vertex_name);
	}
	// remove attribute values
	for (std::map<std::string,std::map<vertex_id,std::string> >::iterator it=vertex_string_attribute.begin(); it!=vertex_string_attribute.end(); it++)
		vertex_string_attribute[it->first].erase(vid);
	for (std::map<std::string,std::map<vertex_id,double> >::iterator it=vertex_numeric_attribute.begin(); it!=vertex_numeric_attribute.end(); it++)
		vertex_numeric_attribute[it->first].erase(vid);
	return true;
}

bool Network::deleteVertex(const std::string& vertex_name) {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name)) return false;
	deleteVertex(vertex_name_to_id[vertex_name]);
	return true;
}

bool Network::deleteEdge(vertex_id vid1, vertex_id vid2) {
	if (!containsEdge(vid1,vid2)) return false;
	edge_id eid(vid1,vid2,isDirected());
	out_edges[vid1].erase(vid2);
	in_edges[vid2].erase(vid1);
	if (!isDirected()) {
		out_edges[vid2].erase(vid1);
		in_edges[vid1].erase(vid2);
	}
	// remove attribute values (including WEIGHT, if present, which is treated as any other numeric attribute)
	for (std::map<std::string,std::map<edge_id,std::string> >::iterator it=edge_string_attribute.begin(); it!=edge_string_attribute.end(); it++)
		edge_string_attribute[it->first].erase(eid);
	for (std::map<std::string,std::map<edge_id,double> >::iterator it=edge_numeric_attribute.begin(); it!=edge_numeric_attribute.end(); it++)
		edge_numeric_attribute[it->first].erase(eid);
	num_edges--;
	return true;
}

bool Network::deleteEdge(const std::string& vertex_name1, const std::string& vertex_name2) {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsEdge(vertex_name1,vertex_name2)) return false;
	return deleteEdge(vertex_name_to_id.at(vertex_name1), vertex_name_to_id.at(vertex_name2));
}

bool Network::containsVertex(vertex_id vid) const {
	return vertexes.count(vid)>0;
}

bool Network::containsVertex(const std::string& vertex_name) const {
	return vertex_name_to_id.count(vertex_name);
}

bool Network::containsEdge(vertex_id vid1, vertex_id vid2) const {
	if (out_edges.count(vid1)==0) return false;
	return out_edges.at(vid1).count(vid2)>0;
}

bool Network::containsEdge(const std::string& vertex_name1, const std::string& vertex_name2) const {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named edge in an unnamed network");
	if (!containsVertex(vertex_name1)) return false;
	if (!containsVertex(vertex_name2)) return false;
	return containsEdge(vertex_name_to_id.at(vertex_name1),vertex_name_to_id.at(vertex_name2));
}

bool Network::isDirected() const  {
	return is_directed;
}

bool Network::isWeighed() const  {
	return is_weighed;
}

bool Network::isNamed() const  {
	return is_named;
}

std::string Network::getVertexName(vertex_id vid) const  {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vid)) throw ElementNotFoundException("Vertex " + std::to_string(vid));
	return vertex_id_to_name.at(vid);
}

vertex_id Network::getVertexId(const std::string& vertex_name) const  {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name)) throw ElementNotFoundException("Vertex " + vertex_name);
	return vertex_name_to_id.at(vertex_name);
}

std::set<vertex_id> Network::getVertexes() const {
	std::set<vertex_id> vertexes;
	vertexes = this->vertexes;
	return vertexes;
}

std::set<edge_id> Network::getEdges() const {
	std::set<edge_id> edges;
	std::map<vertex_id,std::set<vertex_id> >::const_iterator from_edge_iterator;
	std::set<vertex_id>::iterator to_edge_iterator;
	for (from_edge_iterator=out_edges.begin(); from_edge_iterator!=out_edges.end(); ++from_edge_iterator) {
		vertex_id from = from_edge_iterator->first;
		for (to_edge_iterator=from_edge_iterator->second.begin(); to_edge_iterator!=from_edge_iterator->second.end(); ++to_edge_iterator) {
			vertex_id to = (*to_edge_iterator);
			edges.insert(edge_id(from,to,isDirected()));
		}
	}
	return edges;
}

long Network::getNumVertexes() const  {
	return vertexes.size();
}

long Network::getNumEdges() const  {
	return num_edges;
}

long Network::getOutDegree(vertex_id vid) const  {
	if (!containsVertex(vid)) throw ElementNotFoundException("Vertex " + std::to_string(vid));
	if (out_edges.count(vid)==0) return 0;
	return out_edges.at(vid).size();
}

long Network::getInDegree(vertex_id vid) const  {
	if (!containsVertex(vid)) throw ElementNotFoundException("Vertex " + std::to_string(vid));
	if (in_edges.count(vid)==0) return 0;
	return in_edges.at(vid).size();
}

long Network::getDegree(vertex_id vid) const  {
	long tmp_degree = getInDegree(vid);
	if (isDirected()) tmp_degree += getOutDegree(vid);
	return tmp_degree;
}

long Network::getOutDegree(const std::string& vertex_name) const  {
	return getOutDegree(getVertexId(vertex_name));
}

long Network::getInDegree(const std::string& vertex_name) const  {
	return getInDegree(getVertexId(vertex_name));
}

long Network::getDegree(const std::string& vertex_name) const  {
	return getDegree(getVertexId(vertex_name));
}

std::set<vertex_id> Network::getOutNeighbors(vertex_id vid) const  {
	std::set<vertex_id> neighbors;
	if (!containsVertex(vid)) throw ElementNotFoundException("Vertex " + std::to_string(vid));
	std::set<vertex_id>::iterator it;
	if (out_edges.count(vid)!=0)
		for (it=out_edges.at(vid).begin(); it!=out_edges.at(vid).end(); it++)
			neighbors.insert(*it);
	return neighbors;
}

std::set<vertex_id> Network::getInNeighbors(vertex_id vid) const  {
	std::set<vertex_id> neighbors;
	if (!containsVertex(vid)) throw ElementNotFoundException("Vertex " + std::to_string(vid));
	std::set<vertex_id>::const_iterator it;
	if (in_edges.count(vid)!=0)
		for (it=in_edges.at(vid).begin(); it!=in_edges.at(vid).end(); it++)
			neighbors.insert(*it);
	return neighbors;
}

std::set<vertex_id> Network::getNeighbors(vertex_id vid) const  {
	std::set<vertex_id> neighbors;
	if (!containsVertex(vid)) throw ElementNotFoundException("Vertex " + std::to_string(vid));
	std::set<vertex_id>::iterator it;
	if (out_edges.count(vid)!=0)
		for (it=out_edges.at(vid).begin(); it!=out_edges.at(vid).end(); it++)
			neighbors.insert(*it);
	if (isDirected() && in_edges.count(vid)!=0)
			for (it=in_edges.at(vid).begin(); it!=in_edges.at(vid).end(); it++)
				neighbors.insert(*it);
	return neighbors;
}

std::set<std::string> Network::getOutNeighbors(const std::string& vertex_name) const  {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name)) throw ElementNotFoundException("Vertex " + vertex_name);
	std::set<std::string> neighbors;
	std::set<vertex_id> tmp_id_result = getOutNeighbors(vertex_name_to_id.at(vertex_name));
	for (std::set<vertex_id>::iterator it=tmp_id_result.begin(); it!=tmp_id_result.end(); it++)
		neighbors.insert(vertex_id_to_name.at(*it));
	return neighbors;
}

std::set<std::string> Network::getInNeighbors(const std::string& vertex_name) const  {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name)) throw ElementNotFoundException("Vertex " + vertex_name);
	std::set<std::string> neighbors;
	std::set<vertex_id> tmp_id_result = getInNeighbors(vertex_name_to_id.at(vertex_name));
	for (std::set<vertex_id>::iterator it=tmp_id_result.begin(); it!=tmp_id_result.end(); it++)
		neighbors.insert(vertex_id_to_name.at(*it));
	return neighbors;
}

std::set<std::string> Network::getNeighbors(const std::string& vertex_name) const  {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name)) throw ElementNotFoundException("Vertex " + vertex_name);
	std::set<std::string> neighbors;
	std::set<vertex_id> tmp_id_result = getOutNeighbors(vertex_name_to_id.at(vertex_name));
	for (std::set<vertex_id>::iterator it=tmp_id_result.begin(); it!=tmp_id_result.end(); it++)
		neighbors.insert(vertex_id_to_name.at(*it));
	if (isDirected()) {
		std::set<vertex_id> tmp_id_result = getInNeighbors(vertex_name_to_id.at(vertex_name));
		for (std::set<vertex_id>::iterator it=tmp_id_result.begin(); it!=tmp_id_result.end(); it++)
			neighbors.insert(vertex_id_to_name.at(*it));
	}
	return neighbors;
}

double Network::getEdgeWeight(vertex_id vid1, vertex_id vid2) const {
	if (!isWeighed()) throw OperationNotSupportedException("Network is unweighed");
	return getNumericEdgeAttribute(vid1, vid2, "weight");
}

void Network::setEdgeWeight(vertex_id vid1, vertex_id vid2, double weight) {
	if (!isWeighed()) throw OperationNotSupportedException("Network is unweighed");
	setNumericEdgeAttribute(vid1, vid2, "weight", weight);
}

double Network::getEdgeWeight(const std::string& vertex_name1, const std::string& vertex_name2) const {
	if (!isWeighed()) throw OperationNotSupportedException("Network is unweighed");
	return getNumericEdgeAttribute(vertex_name1, vertex_name2, "weight");
}

void Network::setEdgeWeight(const std::string& vertex_name1, const std::string& vertex_name2, double weight) {
	if (!isWeighed()) throw OperationNotSupportedException("Network is unweighed");
	setNumericEdgeAttribute(vertex_name1, vertex_name2, "weight", weight);
}

bool Network::hasVertexAttribute(const std::string& attribute_name) {
	return (vertex_numeric_attribute.count(attribute_name)>0) ||
			(vertex_string_attribute.count(attribute_name)>0);
}

bool Network::hasEdgeAttribute(const std::string& attribute_name) {
	return (edge_numeric_attribute.count(attribute_name)>0) ||
			(edge_string_attribute.count(attribute_name)>0);
}

void Network::newNumericVertexAttribute(const std::string& attribute_name) {
	if (vertex_numeric_attribute.count(attribute_name)>0) throw DuplicateElementException("Attribute " + attribute_name);
	vertex_numeric_attribute[attribute_name] = std::map<vertex_id,double>();
}

void Network::newStringVertexAttribute(const std::string& attribute_name) {
	if (vertex_string_attribute.count(attribute_name)>0) throw DuplicateElementException("Attribute " + attribute_name);
	vertex_string_attribute[attribute_name] = std::map<vertex_id,std::string>();
}

void Network::newNumericEdgeAttribute(const std::string& attribute_name) {
	if (edge_numeric_attribute.count(attribute_name)>0) throw DuplicateElementException("Attribute " + attribute_name);
	edge_numeric_attribute[attribute_name] = std::map<edge_id,double>();
}

void Network::newStringEdgeAttribute(const std::string& attribute_name) {
	if (edge_string_attribute.count(attribute_name)>0) throw DuplicateElementException("Attribute " + attribute_name);
	edge_string_attribute[attribute_name] = std::map<edge_id,std::string>();
}

double Network::getNumericVertexAttribute(vertex_id vid, const std::string& attribute_name) const {
	if (vertex_numeric_attribute.count(attribute_name)==0) throw ElementNotFoundException("Attribute " + attribute_name);
	if (vertex_numeric_attribute.at(attribute_name).count(vid)==0)  throw ElementNotFoundException("No attribute value for attribute " + attribute_name + " on vertex " + std::to_string(vid));
	return vertex_numeric_attribute.at(attribute_name).at(vid);
}

double Network::getNumericVertexAttribute(const std::string& vertex_name, const std::string& attribute_name) const {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name)) throw ElementNotFoundException("Vertex " + vertex_name);
	return getNumericVertexAttribute(vertex_name_to_id.at(vertex_name), attribute_name);
}

std::string Network::getStringVertexAttribute(vertex_id vid, const std::string& attribute_name) const {
	if (vertex_string_attribute.count(attribute_name)==0) throw ElementNotFoundException("Attribute " + attribute_name);
	if (vertex_string_attribute.at(attribute_name).count(vid)==0)  throw ElementNotFoundException("No attribute value for attribute " + attribute_name + " on vertex " + std::to_string(vid));
	return vertex_string_attribute.at(attribute_name).at(vid);
}

std::string Network::getStringVertexAttribute(const std::string& vertex_name, const std::string& attribute_name) const {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name)) throw ElementNotFoundException("Vertex " + vertex_name);
	return getStringVertexAttribute(vertex_name_to_id.at(vertex_name), attribute_name);
}

double Network::getNumericEdgeAttribute(vertex_id vid1, vertex_id vid2, const std::string& attribute_name) const {
	if (!containsVertex(vid1)) throw ElementNotFoundException("Vertex " + std::to_string(vid1));
	if (!containsVertex(vid2)) throw ElementNotFoundException("Vertex " + std::to_string(vid2));
	if (!containsEdge(vid1, vid2)) throw ElementNotFoundException("Edge (" + std::to_string(vid1) + ", " + std::to_string(vid2) + ")");
	if (edge_numeric_attribute.count(attribute_name)==0) throw ElementNotFoundException("Attribute " + attribute_name);
	edge_id eid(vid1, vid2, isDirected());
	if (edge_numeric_attribute.at(attribute_name).count(eid)==0)  throw ElementNotFoundException("No attribute value for attribute " + attribute_name + " on edge (" + std::to_string(vid1) + "," +  std::to_string(vid2) + ")");
	return edge_numeric_attribute.at(attribute_name).at(eid);
}

double Network::getNumericEdgeAttribute(const std::string& vertex_name1, const std::string& vertex_name2, const std::string& attribute_name) const {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name1)) throw ElementNotFoundException("Vertex " + vertex_name1);
	if (!containsVertex(vertex_name2)) throw ElementNotFoundException("Vertex " + vertex_name2);
	return getNumericEdgeAttribute(vertex_name_to_id.at(vertex_name1), vertex_name_to_id.at(vertex_name2), attribute_name);
}

std::string Network::getStringEdgeAttribute(vertex_id vid1, vertex_id vid2, const std::string& attribute_name) const {
	if (!containsVertex(vid1)) throw ElementNotFoundException("Vertex " + std::to_string(vid1));
	if (!containsVertex(vid2)) throw ElementNotFoundException("Vertex " + std::to_string(vid2));
	if (!containsEdge(vid1, vid2)) throw ElementNotFoundException("Edge (" + std::to_string(vid1) + ", " + std::to_string(vid2) + ")");
	if (edge_string_attribute.count(attribute_name)==0) throw ElementNotFoundException("Attribute " + attribute_name);
	edge_id eid(vid1, vid2, isDirected());
	if (edge_string_attribute.at(attribute_name).count(eid)==0)  throw ElementNotFoundException("No attribute value for attribute " + attribute_name + " on edge (" + std::to_string(vid1) + "," +  std::to_string(vid2) + ")");
	return edge_string_attribute.at(attribute_name).at(eid);
}

std::string Network::getStringEdgeAttribute(const std::string& vertex_name1, const std::string& vertex_name2, const std::string& attribute_name) const {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name1)) throw ElementNotFoundException("Vertex " + vertex_name1);
	if (!containsVertex(vertex_name2)) throw ElementNotFoundException("Vertex " + vertex_name2);
	return getStringEdgeAttribute(vertex_name_to_id.at(vertex_name1), vertex_name_to_id.at(vertex_name2), attribute_name);
}

void Network::setNumericVertexAttribute(vertex_id vid, const std::string& attribute_name, double val) {
	if (vertex_numeric_attribute.count(attribute_name)==0) throw ElementNotFoundException("Attribute " + attribute_name);
	vertex_numeric_attribute.at(attribute_name)[vid] = val;
}

void Network::setNumericVertexAttribute(const std::string& vertex_name, const std::string& attribute_name, double val) {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	setNumericVertexAttribute(vertex_name_to_id[vertex_name], attribute_name, val);
}

void Network::setStringVertexAttribute(vertex_id vid, const std::string& attribute_name, const std::string& val) {
	if (vertex_string_attribute.count(attribute_name)==0) throw ElementNotFoundException("Attribute " + attribute_name);
	vertex_string_attribute.at(attribute_name)[vid] = val;
}

void Network::setStringVertexAttribute(const std::string& vertex_name, const std::string& attribute_name, const std::string& val) {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	setStringVertexAttribute(vertex_name_to_id[vertex_name], attribute_name, val);
}

void Network::setNumericEdgeAttribute(vertex_id vid1, vertex_id vid2, const std::string& attribute_name, double val) {
	if (!containsVertex(vid1)) throw ElementNotFoundException("Vertex " + std::to_string(vid1));
	if (!containsVertex(vid2)) throw ElementNotFoundException("Vertex " + std::to_string(vid2));
	if (!containsEdge(vid1, vid2)) throw ElementNotFoundException("Edge (" + std::to_string(vid2) + ", " + std::to_string(vid2) + ")");
	if (edge_numeric_attribute.count(attribute_name)==0) throw ElementNotFoundException("Attribute " + attribute_name);
	edge_id eid(vid1, vid2, isDirected());
	edge_numeric_attribute.at(attribute_name)[eid] = val;
}

void Network::setNumericEdgeAttribute(const std::string& vertex_name1, const std::string& vertex_name2, const std::string& attribute_name, double val) {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name1)) throw ElementNotFoundException("Vertex " + vertex_name1);
	if (!containsVertex(vertex_name2)) throw ElementNotFoundException("Vertex " + vertex_name2);
	setNumericEdgeAttribute(vertex_name_to_id.at(vertex_name1), vertex_name_to_id.at(vertex_name2), attribute_name, val);
}

void Network::setStringEdgeAttribute(vertex_id vid1, vertex_id vid2, const std::string& attribute_name, const std::string& val) {
	if (!containsVertex(vid1)) throw ElementNotFoundException("Vertex " + std::to_string(vid1));
	if (!containsVertex(vid2)) throw ElementNotFoundException("Vertex " + std::to_string(vid2));
	if (!containsEdge(vid1, vid2)) throw ElementNotFoundException("Edge (" + std::to_string(vid2) + ", " + std::to_string(vid2) + ")");
	if (edge_string_attribute.count(attribute_name)==0) throw ElementNotFoundException("Attribute " + attribute_name);
	edge_id eid(vid1, vid2, isDirected());
	edge_string_attribute.at(attribute_name)[eid] = val;
}

void Network::setStringEdgeAttribute(const std::string& vertex_name1, const std::string& vertex_name2, const std::string& attribute_name, const std::string& val) {
	if (!isNamed()) throw OperationNotSupportedException("Cannot reference a named vertex in an unnamed network");
	if (!containsVertex(vertex_name1)) throw ElementNotFoundException("Vertex " + vertex_name1);
	if (!containsVertex(vertex_name2)) throw ElementNotFoundException("Vertex " + vertex_name2);
	setStringEdgeAttribute(vertex_name_to_id.at(vertex_name1), vertex_name_to_id.at(vertex_name2), attribute_name, val);
}

void print(Network& net) {
	std::cout << (net.isDirected()?"DIRECTED":"UNDIRECTED") << " " << (net.isWeighed()?"WEIGHTED":"UNWEIGHTED") << " " << (net.isNamed()?"NAMED":"UNNAMED") << std::endl;
	std::cout << "Number of vertexes: " << net.getNumVertexes() << std::endl;
	std::cout << "Number of edges: " << net.getNumEdges() << std::endl;
}
