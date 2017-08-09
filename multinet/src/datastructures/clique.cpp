#include "datastructures.h"

namespace mlnet {

clique::clique() {}

clique::clique(const std::unordered_set<ActorSharedPtr>& actors, const std::unordered_set<LayerSharedPtr>& layers) :
	actors(actors.begin(),actors.end()), layers(layers.begin(),layers.end()) {}

bool clique::operator==(const clique& comp) const {
	if (actors.size() != comp.actors.size() || layers.size() != comp.layers.size()) return false;
	sorted_set<ActorSharedPtr>::iterator it1 = actors.begin();
	sorted_set<ActorSharedPtr>::iterator it2 = comp.actors.begin();
	for (size_t i = 0; i<actors.size(); i++) {
		if ((*it1)!=(*it2))
			return false;
		++it1; ++it2;
	}
	sorted_set<LayerSharedPtr>::iterator itl1 = layers.begin();
	sorted_set<LayerSharedPtr>::iterator itl2 = comp.layers.begin();
	for (size_t i = 0; i<layers.size(); i++) {
		if ((*itl1)!=(*itl2))
			return false;
		++itl1; ++itl2;
	}
	return true;
}

bool clique::operator!=(const clique& comp) const {
	if (actors.size() != comp.actors.size() || layers.size() != comp.layers.size()) return true;
	sorted_set<ActorSharedPtr>::iterator it1 = actors.begin();
	sorted_set<ActorSharedPtr>::iterator it2 = comp.actors.begin();
	for (size_t i = 0; i<actors.size(); i++) {
		if ((*it1)!=(*it2))
			return true;
		++it1; ++it2;
	}
	sorted_set<LayerSharedPtr>::iterator itl1 = layers.begin();
	sorted_set<LayerSharedPtr>::iterator itl2 = comp.layers.begin();
	for (size_t i = 0; i<layers.size(); i++) {
		if ((*itl1)!=(*itl2))
			return true;
		++itl1; ++itl2;
	}
	return false;
}

bool clique::operator<(const clique& comp) const {
	if (actors.size() != comp.actors.size()) return actors.size() < comp.actors.size();
	if (layers.size() != comp.layers.size()) return layers.size() < comp.layers.size();
	sorted_set<ActorSharedPtr>::iterator it1 = actors.begin();
	sorted_set<ActorSharedPtr>::iterator it2 = comp.actors.begin();
	for (size_t i = 0; i<actors.size(); i++) {
		if ((*it1)<(*it2))
			return true;
		if ((*it1)>(*it2))
			return false;
		++it1; ++it2;
	}
	sorted_set<LayerSharedPtr>::iterator itl1 = layers.begin();
	sorted_set<LayerSharedPtr>::iterator itl2 = comp.layers.begin();
	for (size_t i = 0; i<layers.size(); i++) {
		if ((*itl1)<(*itl2))
			return true;
		if ((*itl1)>(*itl2))
			return false;
		++itl1; ++itl2;
	}
	return false;
}

bool clique::operator>(const clique& comp) const {
	if (actors.size() != comp.actors.size()) return actors.size() > comp.actors.size();
	if (layers.size() != comp.layers.size()) return layers.size() > comp.layers.size();
	sorted_set<ActorSharedPtr>::iterator it1 = actors.begin();
	sorted_set<ActorSharedPtr>::iterator it2 = comp.actors.begin();
	for (size_t i = 0; i<actors.size(); i++) {
		if ((*it1)>(*it2))
			return true;
		if ((*it1)<(*it2))
			return false;
		++it1; ++it2;
	}
	sorted_set<LayerSharedPtr>::iterator itl1 = layers.begin();
	sorted_set<LayerSharedPtr>::iterator itl2 = comp.layers.begin();
	for (size_t i = 0; i<layers.size(); i++) {
		if ((*itl1)>(*itl2))
			return true;
		if ((*itl1)<(*itl2))
			return false;
		++itl1; ++itl2;
	}
	return false;
}

string clique::to_string() {
	std::ostringstream ss;
    ss << "{ ";
    for (ActorSharedPtr actor: actors)
    	ss << actor->name << " ";
    ss << "} + ";
    ss << "[ ";
    for (LayerSharedPtr layer: layers)
    	ss << layer->name << " ";
    ss << "]";
    return ss.str();
}
    
    CliqueSharedPtr clique::create() {
        CliqueSharedPtr result(new clique());
        return result;
    }

    CliqueSharedPtr clique::create(const std::unordered_set<ActorSharedPtr>& actors, const std::unordered_set<LayerSharedPtr>& layers) {
        CliqueSharedPtr result(new clique(actors, layers));
        return result;
    }
    
    
}
