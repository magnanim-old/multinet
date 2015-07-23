#include "datastructures.h"

// In this implementation it is assumed that the map wrapped by this class
// does not change while the iterator is used.

namespace mlnet {

layer_list::layer_list() : data(std::map<layer_id, LayerSharedPtr>()) {}

layer_list::layer_list(const std::map<layer_id,LayerSharedPtr>& data) : data(data) {}

layer_list::~layer_list() {}

layer_list::iterator layer_list::begin() const {
	return iterator(data.begin());
}

layer_list::iterator layer_list::end() const {
	return iterator(data.end());
}

LayerSharedPtr layer_list::iterator::operator*() {
	return current->second;
}

layer_list::iterator::iterator(const std::map<layer_id,LayerSharedPtr>::const_iterator& iter) : current(iter) {
}

layer_list::iterator layer_list::iterator::operator++() { // PREFIX
	current++;
	return *this;
}

layer_list::iterator layer_list::iterator::operator++(int) { // POSTFIX
	layer_list::iterator tmp(*this);
	current++;
	return tmp;
}

bool layer_list::iterator::operator==(const layer_list::iterator& rhs) {
	return current == rhs.current;
}

bool layer_list::iterator::operator!=(const layer_list::iterator& rhs) {
	return current != rhs.current;
}

int layer_list::size() {
	return data.size();
}

}

