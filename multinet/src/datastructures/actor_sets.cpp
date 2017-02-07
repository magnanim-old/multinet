#include "datastructures.h"

namespace mlnet {

actor_set::actor_set() {}

actor_set::actor_set(const std::unordered_set<ActorSharedPtr>& actors) :
	actors(actors.begin(),actors.end()) {}

bool actor_set::operator==(const actor_set& comp) const {
	if (actors.size() != comp.actors.size()) return false;
	sorted_set<ActorSharedPtr>::iterator it1 = actors.begin();
	sorted_set<ActorSharedPtr>::iterator it2 = comp.actors.begin();
	for (uint i = 0; i<actors.size(); i++) {
		if ((*it1)!=(*it2))
			return false;
		++it1; ++it2;
	}
	return true;
}

bool actor_set::operator!=(const actor_set& comp) const {
	if (actors.size() != comp.actors.size()) return true;
	sorted_set<ActorSharedPtr>::iterator it1 = actors.begin();
	sorted_set<ActorSharedPtr>::iterator it2 = comp.actors.begin();
	for (uint i = 0; i<actors.size(); i++) {
		if ((*it1)!=(*it2))
			return true;
		++it1; ++it2;
	}
	return false;
}

bool actor_set::operator<(const actor_set& comp) const {
	if (actors.size() != comp.actors.size()) return actors.size() < comp.actors.size();
	sorted_set<ActorSharedPtr>::iterator it1 = actors.begin();
	sorted_set<ActorSharedPtr>::iterator it2 = comp.actors.begin();
	for (uint i = 0; i<actors.size(); i++) {
		if ((*it1)<(*it2))
			return true;
		if ((*it1)>(*it2))
			return false;
		++it1; ++it2;
	}
	return false;
}

bool actor_set::operator>(const actor_set& comp) const {
	if (actors.size() != comp.actors.size()) return actors.size() > comp.actors.size();
	sorted_set<ActorSharedPtr>::iterator it1 = actors.begin();
	sorted_set<ActorSharedPtr>::iterator it2 = comp.actors.begin();
	for (uint i = 0; i<actors.size(); i++) {
		if ((*it1)>(*it2))
			return true;
		if ((*it1)<(*it2))
			return false;
		++it1; ++it2;
	}
	return false;
}

dyad::dyad(const ActorSharedPtr& actor1, const ActorSharedPtr& actor2) {
	actors.insert(actor1);
	actors.insert(actor2);
}

triad::triad(const ActorSharedPtr& actor1, const ActorSharedPtr& actor2, const ActorSharedPtr& actor3) {
	actors.insert(actor1);
	actors.insert(actor2);
	actors.insert(actor3);
}

string actor_set::to_string() {
	return mlnet::set_to_string(actors.begin(),actors.end());
}

}
