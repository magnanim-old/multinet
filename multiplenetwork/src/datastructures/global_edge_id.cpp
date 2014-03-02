#include "datastructures.h"

global_edge_id::global_edge_id(vertex_id v1, vertex_id v2, bool directed, network_id network) {
	global_edge_id::v1 = v1;
	global_edge_id::v2 = v2;
	global_edge_id::directed = directed;
	global_edge_id::network = network;
}

bool global_edge_id::operator==(const global_edge_id& e2) const {
    return (network==e2.network) && (((v1==e2.v1)&&(v2==e2.v2)) ||
    		(!directed && (v1==e2.v2) && (v2==e2.v1)));
}

/*
bool edge_id::operator<=(const edge_id& e2) const {
    if (v1<e2.v1) return true;
    if (v1==e2.v1) {
    	if (v2<e2.v2) return true;
    	if (v2==e2.v2) {
    		if (network<=e2.network) return true;
    	}
    }
    return false;
}
*/

bool global_edge_id::operator<(const global_edge_id& e2) const {
    if (v1<e2.v1) return true;
    if (v1==e2.v1) {
    	if (v2<e2.v2) return true;
    	 if (v2==e2.v2) {
    		 if (network<e2.network) return true;
    	 }
    }
    return false;
}

