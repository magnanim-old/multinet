#include "datastructures.h"

// In this implementation it is assumed that the map wrapped by this class
// does not change while the iterator is used.

namespace mlnet {


node_list::node_list() : data(std::map<node_id, NodeSharedPtr>()) {}

node_list::node_list(const std::map<node_id,NodeSharedPtr>& data) : data(data) {}

node_list::~node_list() {}

node_list::iterator node_list::begin() const {
	return iterator(data.begin());
}

node_list::iterator node_list::end() const {
	return iterator(data.end());
}

NodeSharedPtr node_list::iterator::operator*() {
	return current->second;
}

node_list::iterator::iterator(const std::map<node_id,NodeSharedPtr>::const_iterator& iter) : current(iter) {
}

node_list::iterator node_list::iterator::operator++() { // PREFIX
	current++;
	return *this;
}

node_list::iterator node_list::iterator::operator++(int) { // POSTFIX
	node_list::iterator tmp(*this);
	current++;
	return tmp;
}

bool node_list::iterator::operator==(const node_list::iterator& rhs) {
	return current == rhs.current;
}

bool node_list::iterator::operator!=(const node_list::iterator& rhs) {
	return current != rhs.current;
}

int node_list::size() {
	return data.size();
}

}

