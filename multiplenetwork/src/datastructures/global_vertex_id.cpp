#include "datastructures.h"
#include <sstream>

global_vertex_id::global_vertex_id(vertex_id vid, network_id network) :
	vid(vid),
	network(network) {}

bool global_vertex_id::operator==(const global_vertex_id& e) const {
    return (network==e.network) && (vid==e.vid);
}

bool global_vertex_id::operator!=(const global_vertex_id& e) const {
    return ! operator==(e);
}

bool global_vertex_id::operator<(const global_vertex_id& e) const {
    if (vid<e.vid) return true;
    else if (vid>e.vid) return false;

    if (network<e.network) return true;

    return false;
}

bool global_vertex_id::operator>(const global_vertex_id& e) const {
    return ! operator<(e) && ! operator==(e);
}

std::ostream& operator<<(std::ostream &strm, const global_vertex_id& global_id) {
	strm << "[V:" << global_id.vid << ",N:" << global_id.network << "]";
	return strm;
}

std::string global_vertex_id::to_string() const {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

