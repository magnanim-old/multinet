/*
 * MultiDistance.cpp
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

/**
 * @param p a MultiDistance that is cloned to initialize the new one
 * @param timestamp creation timestamp
 *
Distance::Distance(const Distance& p, long timestamp) {
	Distance::mnet = p.mnet;
	num_edges_per_layer.insert(num_edges_per_layer.begin(),p.num_edges_per_layer.begin(),p.num_edges_per_layer.end());
	Distance::timestamp = timestamp;
	num_steps = p.num_steps;
}

 only for debugging *
Distance::Distance(long num_layers, long timestamp) {
	Distance::num_edges_per_layer.resize(num_layers,0);
	Distance::timestamp = timestamp;
	num_steps = 0;
}

Distance::Distance(const MultipleNetwork& mnet, long timestamp) {
	Distance::mnet = (&mnet);
	Distance::num_edges_per_layer.resize(mnet.getNumNetworks(),0);
	Distance::timestamp = timestamp;
	num_steps = 0;
}

Distance::~Distance() {
	// TODO Auto-generated destructor stub
}

void Distance::extend(network_id nid) {
	num_edges_per_layer[nid]++;
	num_steps++;
}

long Distance::getTimestamp() const {
	return timestamp;
}

long Distance::getNumNetworks() const {
	return num_edges_per_layer.size();
}

long Distance::getNumEdgesOnNetwork(long layer) const {
	return num_edges_per_layer[layer];
}

long Distance::length() const {
	return num_steps;
}

bool Distance::operator<(const Distance& other) const {
	return timestamp<other.getTimestamp();
}

Distance Distance::operator=(const Distance& other) const {
	return Distance(other, other.timestamp);
}

bool Distance::same(const Distance& other) const {
	if (num_edges_per_layer!=other.num_edges_per_layer) return false;
	return true;
}

std::ostream& operator<<(std::ostream &strm, const Distance &dist) {
  strm << "Distance [ts=" << dist.getTimestamp() << "](";
  int num_layers = dist.num_edges_per_layer.size();
  for (int i=0; i<num_layers; i++) {
  	  strm << dist.num_edges_per_layer[i];
  	  if (i!=num_layers-1) strm << ",";
  }
  strm << ")";

  return strm;
}*/
