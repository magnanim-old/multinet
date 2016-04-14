#include "datastructures.h"

using namespace std;

namespace mlnet {

path_length::path_length(const MLNetworkSharedPtr& mnet) : mnet(mnet), total_length(0), ts(0) {}

void path_length::step(const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	num_edges.inc(layer1->id,layer2->id);
	total_length++;
}

long path_length::length() const {
	return total_length;
}

long path_length::length(const LayerSharedPtr& layer) const {
	return num_edges.count(layer->id,layer->id);
}

long path_length::length(const LayerSharedPtr& from, const LayerSharedPtr& to) const {
	return num_edges.count(from->id,to->id);
}

comparison_result path_length::compare(const path_length& other, comparison_type comp) const {
	switch (comp) {
	case FULL:
		return compare_full(other);
	case SWITCH_COSTS:
		return compare_switch(other);
	case MULTIPLEX:
		return compare_multiplex(other);
	case SIMPLE:
		return compare_simple(other);
	}
	throw WrongParameterException("Wrong comparison type");
}

comparison_result path_length::compare_full(const path_length& other) const {
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
		    	return INCOMPARABLE;
		}
	}
	if (canDominate && !canBeDominated)
		return GREATER_THAN;
	if (canBeDominated && !canDominate)
		return LESS_THAN;
	//if (canDominate && canBeDominated)
		return EQUAL;
}

comparison_result path_length::compare_switch(const path_length& other) const {
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
			return INCOMPARABLE;
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
		return INCOMPARABLE;
	if (canDominate && !canBeDominated)
		return GREATER_THAN;
	if (canBeDominated && !canDominate)
		return LESS_THAN;
	//if (canDominate && canBeDominated)
		return EQUAL;
}

comparison_result path_length::compare_multiplex(const path_length& other) const {
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
			return INCOMPARABLE;
	}
	if (canDominate && !canBeDominated)
		return GREATER_THAN;
	if (canBeDominated && !canDominate)
		return LESS_THAN;
	//if (canDominate && canBeDominated)
		return EQUAL;
}

comparison_result path_length::compare_simple(const path_length& other) const {
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
		return INCOMPARABLE;
	if (canDominate && !canBeDominated)
		return GREATER_THAN;
	if (canBeDominated && !canDominate)
		return LESS_THAN;
	//if (canDominate && canBeDominated)
		return EQUAL;
}

bool path_length::operator<(const path_length& other) const {
	//return total_length < other.total_length;
	return ts < other.ts;
}

bool path_length::operator>(const path_length& other) const {
	//return total_length > other.total_length;
	return ts > other.ts;
}

bool path_length::operator==(const path_length& other) const {
	//return total_length == other.total_length;
	return ts == other.ts;
}

bool path_length::operator!=(const path_length& other) const {
	//return total_length != other.total_length;
	return ts != other.ts;
}

std::string path_length::to_string() const {
	std::string res;
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
