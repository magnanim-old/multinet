#include "datastructures.h"

// In this implementation it is assumed that the map wrapped by this class
// does not change while the iterator is used.

namespace mlnet {

edge_list::edge_list() : data(std::map<edge_id, EdgeSharedPtr>()) {}

edge_list::edge_list(const std::map<edge_id,EdgeSharedPtr>& data) : data(data) {}

edge_list::~edge_list() {}

edge_list::iterator edge_list::begin() const {
	return iterator(data.begin());
}

edge_list::iterator edge_list::end() const {
	std::map<edge_id,EdgeSharedPtr>::const_iterator internal_iterator = data.end();
	return iterator(internal_iterator);
}

EdgeSharedPtr edge_list::iterator::operator*() {
	return current->second;
}

edge_list::iterator::iterator(const std::map<edge_id,EdgeSharedPtr>::const_iterator& iter) : current(iter) {
}

edge_list::iterator edge_list::iterator::operator++() { // PREFIX
	current++;
	return *this;
}

edge_list::iterator edge_list::iterator::operator++(int) { // POSTFIX
	edge_list::iterator tmp(*this);
	current++;
	return tmp;
}

bool edge_list::iterator::operator==(const edge_list::iterator& rhs) {
	return current == rhs.current;
}

bool edge_list::iterator::operator!=(const edge_list::iterator& rhs) {
	return current != rhs.current;
}

int edge_list::size() {
	return data.size();
}

}

