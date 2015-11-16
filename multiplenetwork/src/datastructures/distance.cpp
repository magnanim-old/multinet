#include "datastructures.h"

using namespace std;

namespace mlnet {

distance::distance(const MLNetworkSharedPtr& mnet) : mnet(mnet), total_length(0), ts(0) {}

void distance::step(const NodeSharedPtr& n1, const NodeSharedPtr& n2) {
	num_edges.inc(n1->layer->id,n2->layer->id);
	total_length++;
}

long distance::length() const {
	return total_length;
}


long distance::length(const LayerSharedPtr& layer) const {
	return num_edges.count(layer->id,layer->id);
}

long distance::length(const LayerSharedPtr& from, const LayerSharedPtr& to) const {
	return num_edges.count(from->id,to->id);
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
			long l1 = length(layer1,layer2);
			long l2 = other.length(layer1,layer2);
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
		long l1 = length(layer1,layer1);
		num_intralayer_steps1 += l1;
		long l2 = other.length(layer1,layer1);
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
		long l1 = length(layer1,layer1);
		num_intralayer_steps1 += l1;
		long l2 = other.length(layer1,layer1);
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
	return ts < other.ts;
}

bool distance::operator>(const distance& other) const {
	return ts > other.ts;
}

bool distance::operator==(const distance& other) const {
	return ts == other.ts;
}

bool distance::operator!=(const distance& other) const {
	return ts != other.ts;
}

std::string distance::to_string() const {
	std::string res = "Steps:";
	for (LayerSharedPtr layer: mnet->get_layers()) {
		long l = length(layer,layer);
		res += " " + layer->name + ":" + std::to_string(l);
	}
	for (LayerSharedPtr layer1: mnet->get_layers()) {
		for (LayerSharedPtr layer2: mnet->get_layers()) {
			if (layer1==layer2)
				continue;
			long l = length(layer1,layer2);
			if (l!=0) {
				res += " " + layer1->name + "->" + layer2->name + ":" + std::to_string(l);
			}
		}
	}
	return res;
}

} // namespace
