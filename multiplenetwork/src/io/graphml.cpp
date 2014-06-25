/*
 * creation.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: magnanim
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include "io.h"
#include "utils.h"
#include "datastructures.h"
#include "exceptions.h"

using namespace std;

void write_graphml(const Network& net, const std::string& path) {
	std::ofstream outfile;
	outfile.open(path.data());
	if (outfile.fail()) throw FileNotFoundException(path);

	bool has_vertex_attributes = net.getStringVertexAttributes().size() + net.getNumericVertexAttributes().size();
	bool has_edge_attributes = net.getStringEdgeAttributes().size() + net.getNumericEdgeAttributes().size();

	outfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	outfile << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"" << std::endl;
	outfile << "    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""  << std::endl;
	outfile << "    xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns" << std::endl;
	outfile << "     http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">" << std::endl;

	int i = 0;
	for (std::string att: net.getStringVertexAttributes()) {
		outfile << "    <key id=\"v" << (i++) << "\" for=\"node\" attr.name=\"" << att << "\" attr.type=\"string\"/>" << std::endl;
	}
	for (std::string att: net.getNumericVertexAttributes()) {
		outfile << "    <key id=\"v" << (i++) << "\" for=\"node\" attr.name=\"" << att << "\" attr.type=\"double\"/>" << std::endl;
	}
	i = 0;
	for (std::string att: net.getStringEdgeAttributes()) {
		outfile << "    <key id=\"e" << (i++) << "\" for=\"edge\" attr.name=\"" << att << "\" attr.type=\"string\"/>" << std::endl;
	}
	for (std::string att: net.getNumericEdgeAttributes()) {
		outfile << "    <key id=\"e" << (i++) << "\" for=\"edge\" attr.name=\"" << att << "\" attr.type=\"double\"/>" << std::endl;
	}

	outfile << "  <graph id=\"G\" edgedefault=\"" << (net.isDirected()?"directed":"undirected") << "\">" << std::endl;

	std::set<vertex_id> vertexes = net.getVertexes();
	for (vertex_id vertex: vertexes) {
		if (has_vertex_attributes) {
			outfile << "    <node id=\"" << net.getVertexName(vertex) << "\">" << std::endl;
			i = 0;
			for (std::string att: net.getStringEdgeAttributes()) {
				outfile << "        <data key=\"v" << (i++) << "\">" << net.getStringVertexAttribute(vertex,att) << "</data>" << std::endl;
			}
			for (std::string att: net.getNumericEdgeAttributes()) {
				outfile << "        <data key=\"v" << (i++) << "\">" << net.getNumericVertexAttribute(vertex,att) << "</data>" << std::endl;
			}
			outfile << "    </node>" << std::endl;
		}
		else outfile << "    <node id=\"" << net.getVertexName(vertex) << "\"/>" << std::endl;
	}
	std::set<edge_id> edges = net.getEdges();
	long eid = 0;
	for (edge_id edge: edges) {
		if (has_edge_attributes) {
			outfile << "    <edge id=\"e" << (eid++) << "\" source=\"" << net.getVertexName(edge.v1) << "\" target=\"" << net.getVertexName(edge.v2) << "\">" << std::endl;
			i = 0;
			for (std::string att: net.getStringEdgeAttributes()) {
				outfile << "        <data key=\"e" << (i++) << "\">" << net.getStringEdgeAttribute(edge.v1,edge.v2,att) << "</data>" << std::endl;
			}
			for (std::string att: net.getNumericEdgeAttributes()) {
				outfile << "        <data key=\"e" << (i++) << "\">" << net.getNumericEdgeAttribute(edge.v1,edge.v2,att) << "</data>" << std::endl;
			}
			outfile << "    </edge>" << std::endl;
		}
		else outfile << "    <edge id=\"e" << (eid++) << "\" source=\"" << net.getVertexName(edge.v1) << "\" target=\"" << net.getVertexName(edge.v2) << "\"/>" << std::endl;
	}
	outfile << "  </graph>" << std::endl;
	outfile << "</graphml>" << std::endl;
}
