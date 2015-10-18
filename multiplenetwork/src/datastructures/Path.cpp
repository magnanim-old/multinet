/*
 * MultiPath.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: magnanim
 */

#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include "datastructures.h"

using namespace std;

namespace mlnet {
/**
 * @param p a MultiPath that is cloned to initialize the new one
 * @param timestamp creation timestamp
 *
Path::Path(const Path& p, long timestamp) {
	Path::mnet = p.mnet;
	num_edges_per_layer.insert(num_edges_per_layer.begin(),p.num_edges_per_layer.begin(),p.num_edges_per_layer.end());
	path.insert(path.begin(),p.path.begin(),p.path.end());
	network.insert(network.begin(),p.network.begin(),p.network.end());
	Path::timestamp = timestamp;
}

only for debugging *
Path::Path(long num_layers, long timestamp) {
	Path::num_edges_per_layer.resize(num_layers,0);
	Path::timestamp = timestamp;
}


path::path(const MLNetworkSharedPtr mnet, NodeSharedPtr origin) : mnet(mnet), origin(origin) {}

path::~path() {}


NodeSharedPtr path::begin() {
	return origin;
}

NodeSharedPtr path::end() {
	if (path.size()==0)
		return origin;
	else return path[path.size()-1]->v2;
}

void path::step(EdgeSharedPtr e) {
	path.push_back(e);
}

EdgeSharedPtr path::get_step(long pos) const {
	return path.at(pos);
}

long path::length() const {
	return path.size();
}

bool Path::operator<(const Path& other) const {}

Path Path::operator=(const Path& other) const {}

bool Path::operator==(const Path& other) const {}*/

/*
void Path::start(vertex_id first) {
	// TODO Check it's empty -> well, or move to constructor...
	path.push_back(first);
}

void Path::extend(vertex_id to, network_id nid) {
	path.push_back(to);
	network.push_back(nid);
	num_edges_per_layer[nid]++;
}

long Path::getTimestamp() const {
	return timestamp;
}

long Path::getNumNetworks() const {
	return num_edges_per_layer.size();
}

long Path::getNumEdgesOnNetwork(long layer) const {
	return num_edges_per_layer[layer];
}

long Path::length() const {
	return network.size();
}

long Path::getVertex(long pos) const {
	//cout << pos << " returning " << edge_id[pos];
	return path[pos];
}

network_id Path::getNetwork(long pos) const {
	return network[pos];
}

bool Path::operator<(const Path& other) const {
	return timestamp<other.getTimestamp();
}

Path Path::operator=(const Path& other) const {
	return Path(other, other.timestamp);
}

bool Path::same(const Path& other) const {
	if (path!=other.path) return false;
	if (network!=other.network) return false;
	return true;
}

std::ostream& operator<<(std::ostream &strm, const Path &path) {
  strm << "Path[ts=" << path.getTimestamp() << "] ";
  for (int i=0; i<path.length(); i++) {
	  strm << path.path[i] << " -" << path.network[i];
	  strm << "-> ";
  }
  strm << path.path[path.length()];


  strm << "\n";
  strm << "- edges per layer: ";
  int num_layers = path.num_edges_per_layer.size();
  for (int i=0; i<num_layers; i++) {
  	  strm << path.num_edges_per_layer[i];
  	  if (i!=num_layers-1) strm << ",";
  }*/
  //strm << "\n";

} // namespace
