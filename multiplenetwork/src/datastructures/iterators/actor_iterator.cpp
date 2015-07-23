#include "datastructures.h"

// In this implementation it is assumed that the map wrapped by this class
// does not change while the iterator is used.

namespace mlnet {

actor_list::actor_list() : data(std::map<actor_id, ActorSharedPtr>()) {}

actor_list::actor_list(const std::map<actor_id,ActorSharedPtr>& data) : data(data) {}

actor_list::~actor_list() {}

actor_list::iterator actor_list::begin() const {
	return iterator(data.begin());
}

actor_list::iterator actor_list::end() const {
	return iterator(data.end());
}

ActorSharedPtr actor_list::iterator::operator*() {
	return current->second;
}

actor_list::iterator::iterator(const std::map<actor_id,ActorSharedPtr>::const_iterator& iter) : current(iter) {
}

actor_list::iterator actor_list::iterator::operator++() { // PREFIX
	current++;
	return *this;
}

actor_list::iterator actor_list::iterator::operator++(int) { // POSTFIX
	actor_list::iterator tmp(*this);
	current++;
	return tmp;
}

bool actor_list::iterator::operator==(const actor_list::iterator& rhs) {
	return current == rhs.current;
}

bool actor_list::iterator::operator!=(const actor_list::iterator& rhs) {
	return current != rhs.current;
}

int actor_list::size() {
	return data.size();
}

}
