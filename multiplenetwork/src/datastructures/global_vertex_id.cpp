#include "datastructures.h"

global_vertex_id::global_vertex_id() {
	vid = -1;
	network = -1;
}

global_vertex_id::global_vertex_id(vertex_id _vid, network_id _network) {
	vid = _vid;
	network = _network;
}

void global_vertex_id::set_vertex_id(vertex_id vid) {
	this->vid = vid;
}

vertex_id global_vertex_id::get_vertex_id() {
	return vid;
}

bool global_vertex_id::operator==(const global_vertex_id& e) const {
    return (network==e.network) &&
    	   (vid==e.vid);
}

bool global_vertex_id::operator<(const global_vertex_id& e) const {
    if (vid<e.vid) return true;
    if (vid==e.vid) {
    	if (network<e.network) return true;
    }
    return false;
}

