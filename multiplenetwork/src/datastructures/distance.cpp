#include "datastructures.h"

using namespace std;

namespace mlnet {

distance::distance(const MLNetworkSharedPtr& mnet) : mnet(mnet), total_length(0) {}

void distance::step(const NodeSharedPtr& n1, const NodeSharedPtr& n2) {
	if (num_edges.count(n1->layer->id)==0 || num_edges.at(n1->layer->id).count(n2->layer->id)==0) {
		num_edges[n1->layer->id][n2->layer->id]=0;
		if (!mnet->is_directed(n1->layer,n2->layer) && n1->layer!=n2->layer) {
			num_edges[n2->layer->id][n1->layer->id]=0;
		}
	}
	num_edges[n1->layer->id][n2->layer->id]++;
	if (!mnet->is_directed(n1->layer,n2->layer) && n1->layer!=n2->layer) {
		num_edges[n2->layer->id][n1->layer->id]++;
	}
	total_length++;
}


long distance::length() const {
	return total_length;
}

long distance::length(const layer_id& from, const layer_id& to) const {
	if (num_edges.count(from)==0 || num_edges.at(from).count(to)==0) {
		return 0;
	}
	else return num_edges.at(from).at(to);
}


domination distance::compare(const distance& other, comparison_type comp) const {
	switch (comp) {
	case FULL_COMPARISON:
		return compare_full(other);
	case SWITCH_COMPARISON:
		return compare_switch(other);
	case MULTIPLEX_COMPARISON:
		return compare_multiplex(other);
	case SIMPLE_COMPARISON:
		return compare_simple(other);
	}
	throw WrongParameterException("Wrong comparison type");
}

domination distance::compare_full(const distance& other) const {
	bool canBeDominated = true;
	bool canDominate = true;
	if (mnet != other.mnet) {
		throw OperationNotSupportedException("Cannot compare distances on different networks");
	}

	for (LayerSharedPtr layer1: mnet->get_layers()) {
		for (LayerSharedPtr layer2: mnet->get_layers()) {
			long l1 = length(layer1->id,layer2->id);
			long l2 = other.length(layer1->id,layer2->id);
		    if (l1 > l2) {
		    	canDominate = false;
		    }
		    else if (l1 < l2) {
		    	canBeDominated = false;
		    }
		    if (!canBeDominated && !canDominate)
		    	return P_INCOMPARABLE;
		}
	}
	if (canDominate && !canBeDominated)
		return P_DOMINATES;
	if (canBeDominated && !canDominate)
		return P_DOMINATED;
	//if (canDominate && canBeDominated)
		return P_EQUAL;
}

domination distance::compare_switch(const distance& other) const {
	bool canBeDominated = true;
	bool canDominate = true;
	if (mnet != other.mnet) {
		throw OperationNotSupportedException("Cannot compare distances on different networks");
	}
	long num_intralayer_steps1 = 0;
	long num_intralayer_steps2 = 0;
	for (LayerSharedPtr layer1: mnet->get_layers()) {
		long l1 = length(layer1->id,layer1->id);
		num_intralayer_steps1 += l1;
		long l2 = other.length(layer1->id,layer1->id);
		num_intralayer_steps2 += l2;
		if (l1 > l2) {
			canDominate = false;
		}
		else if (l1 < l2) {
			canBeDominated = false;
		}
		if (!canBeDominated && !canDominate)
			return P_INCOMPARABLE;
	}
	long num_interlayer_steps1 = length()-num_intralayer_steps1;
	long num_interlayer_steps2 = other.length()-num_intralayer_steps2;
	if (num_interlayer_steps1 > num_interlayer_steps2) {
		canDominate = false;
	}
	else if (num_interlayer_steps1 < num_interlayer_steps2) {
		canBeDominated = false;
	}
	if (!canBeDominated && !canDominate)
		return P_INCOMPARABLE;
	if (canDominate && !canBeDominated)
		return P_DOMINATES;
	if (canBeDominated && !canDominate)
		return P_DOMINATED;
	//if (canDominate && canBeDominated)
		return P_EQUAL;
}

domination distance::compare_multiplex(const distance& other) const {
	bool canBeDominated = true;
	bool canDominate = true;
	if (mnet != other.mnet) {
		throw OperationNotSupportedException("Cannot compare distances on different networks");
	}
	long num_intralayer_steps1 = 0;
	long num_intralayer_steps2 = 0;
	for (LayerSharedPtr layer1: mnet->get_layers()) {
		long l1 = length(layer1->id,layer1->id);
		num_intralayer_steps1 += l1;
		long l2 = other.length(layer1->id,layer1->id);
		num_intralayer_steps2 += l2;
		if (l1 > l2) {
			canDominate = false;
		}
		else if (l1 < l2) {
			canBeDominated = false;
		}
		if (!canBeDominated && !canDominate)
			return P_INCOMPARABLE;
	}
	if (canDominate && !canBeDominated)
		return P_DOMINATES;
	if (canBeDominated && !canDominate)
		return P_DOMINATED;
	//if (canDominate && canBeDominated)
		return P_EQUAL;
}

domination distance::compare_simple(const distance& other) const {
	bool canBeDominated = true;
	bool canDominate = true;
	if (mnet != other.mnet) {
		throw OperationNotSupportedException("Cannot compare distances on different networks");
	}
	long l1 = length();
	long l2 = other.length();
	if (l1 > l2) {
		canDominate = false;
	}
	else if (l1 < l2) {
		canBeDominated = false;
	}
	if (!canBeDominated && !canDominate)
		return P_INCOMPARABLE;
	if (canDominate && !canBeDominated)
		return P_DOMINATES;
	if (canBeDominated && !canDominate)
		return P_DOMINATED;
	//if (canDominate && canBeDominated)
		return P_EQUAL;
}


bool distance::operator<(const distance& other) const {
	return length()<other.length();
}

bool distance::operator>(const distance& other) const {
	return length()>other.length();
}

bool distance::operator==(const distance& other) const {
	return length()==other.length();
}

bool distance::operator!=(const distance& other) const {
	return length()!=other.length();
}

std::string distance::to_string() const {
	std::string res = "Steps:";
	for (LayerSharedPtr layer: mnet->get_layers()) {
		long l = length(layer->id,layer->id);
		res += " " + layer->name + ":" + std::to_string(l);
	}
	for (LayerSharedPtr layer1: mnet->get_layers()) {
		for (LayerSharedPtr layer2: mnet->get_layers()) {
			if (layer1==layer2)
				continue;
			long l = length(layer1->id,layer2->id);
			if (l!=0) {
				res += " " + layer1->name + "->" + layer2->name + ":" + std::to_string(l);
			}
		}
	}
	return res;
}

} // namespace
