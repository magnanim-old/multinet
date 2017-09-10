#include "layout.h"
#include <cmath>

namespace mlnet {

hash_map<NodeSharedPtr,xyz_coordinates> circular(MLNetworkSharedPtr& mnet, double radius) {
	hash_map<NodeSharedPtr,xyz_coordinates> pos;
    double pi = 3.14159265358979323846;
	
	if (mnet->get_actors()->size()==0) return pos;

	double angle_offset = 360.0/mnet->get_actors()->size();
    int i=0;
	for (ActorSharedPtr a: *mnet->get_actors()) {
        double degree = i*angle_offset;
        double radians = degree*pi/180;
        double x = std::cos(radians)*radius;
        double y = std::sin(radians)*radius;
        for (NodeSharedPtr n: *mnet->get_nodes(a)) {
			pos[n].x = x;
			pos[n].y = y;
			pos[n].z = mnet->get_layers()->get_index(n->layer);
		}
        i++;
	}
	return pos;
}

}


