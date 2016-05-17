#include <stdio.h>
#include <vector>
#include <iostream>
#include "layout.h"

namespace mlnet {

double fr(double p, double k) {return k*k/p;}
double fain(double p, double k) {return p*p/k;}
double fainter(double p, double k) {return p*p/k;}

hashtable<NodeSharedPtr,coordinates> multiforce(MLNetworkSharedPtr mnet, double width, double length, int iterations) {
	hashtable<NodeSharedPtr,coordinates> pos;
	hashtable<NodeSharedPtr,coordinates> disp;

	double t = std::sqrt(width*width+length*length);
	double area = width*length;
	double k = std::sqrt(area/mnet->get_actors().size());
	for (ActorSharedPtr a: mnet->get_actors()) {
		double y = drand()*length-length/2;
		double x = drand()*width-width/2;
		for (NodeSharedPtr n: mnet->get_nodes(a)) {
			pos[n].y = x;
			pos[n].x = y;
			pos[n].z = mnet->get_layers().get_index(n->layer->id);
		}
	}

	for (int i=0; i<iterations; i++) {
		// calculate repulsive forces
		for (LayerSharedPtr l: mnet->get_layers()) {
			for (NodeSharedPtr v: mnet->get_nodes(l)) {
				disp[v].x = 0;
				disp[v].y = 0;
				for (NodeSharedPtr u: mnet->get_nodes(l)) {
					if (u == v) continue;
					coordinates Delta;
					Delta.x = pos[v].x - pos[u].x;
					Delta.y = pos[v].y - pos[u].y;
					double DeltaNorm = std::sqrt(Delta.x*Delta.x+Delta.y*Delta.y);
					disp[v].x = disp[v].x + Delta.x/DeltaNorm*fr(DeltaNorm,k);
					disp[v].y = disp[v].y + Delta.y/DeltaNorm*fr(DeltaNorm,k);
				}
			}
		}
		// calculate attractive forces inside each layer for ((u, v) ∈ E) do
		for (LayerSharedPtr l: mnet->get_layers()) {
			for (EdgeSharedPtr e: mnet->get_edges(l,l)) {
				NodeSharedPtr v = e->v1;
				NodeSharedPtr u = e->v2;
				coordinates Delta;
				Delta.x = pos[v].x - pos[u].x;
				Delta.y = pos[v].y - pos[u].y;
				double DeltaNorm = std::sqrt(Delta.x*Delta.x+Delta.y*Delta.y);
				disp[v].x = disp[v].x - Delta.x/DeltaNorm*fain(DeltaNorm,k);
				disp[v].y = disp[v].y - Delta.y/DeltaNorm*fain(DeltaNorm,k);
				disp[u].x = disp[u].x + Delta.x/DeltaNorm*fain(DeltaNorm,k);
				disp[u].y = disp[u].y + Delta.y/DeltaNorm*fain(DeltaNorm,k);
			}
		}
		// calculate attractive forces across layers
		for (ActorSharedPtr a: mnet->get_actors()) {
			for (NodeSharedPtr v: mnet->get_nodes(a)) {
				for (NodeSharedPtr u: mnet->get_nodes(a)) {
					if (v == u) continue;
					coordinates Delta;
					Delta.x = pos[v].x - pos[u].x;
					Delta.y = pos[v].y - pos[u].y;
					double DeltaNorm = std::sqrt(Delta.x*Delta.x+Delta.y*Delta.y);
					disp[v].x = disp[v].x - Delta.x/DeltaNorm*fainter(DeltaNorm,k);
					disp[v].y = disp[v].y - Delta.y/DeltaNorm*fainter(DeltaNorm,k);
					disp[u].x = disp[u].x + Delta.x/DeltaNorm*fainter(DeltaNorm,k);
					disp[u].y = disp[u].y + Delta.y/DeltaNorm*fainter(DeltaNorm,k);
				}
			}
		}
		// assign new positions
		for (NodeSharedPtr v: mnet->get_nodes()) {
			double dispNorm = std::sqrt(disp[v].x*disp[v].x+disp[v].y*disp[v].y);
			pos[v].x = pos[v].x + (disp[v].x/dispNorm);//*std::min(dispNorm,t);
			pos[v].y = pos[v].y + (disp[v].y/dispNorm);//*std::min(dispNorm,t);
			//pos[v].x = std::min(width/2, std::max(-width/2, pos[v].x));
			//pos[v].y = std::min(length/2, std::max(-length/2, pos[v].y));
		}
		// reduce the temperature
		t -= (t-1)/(iterations+1);
	}

	// DEBUG PRINT
	double min_x = 100000000;
	double min_y = 100000000;
	double max_x = -100000000;
	double max_y = -100000000;
	for (NodeSharedPtr v: mnet->get_nodes()) {
		if (pos[v].x < min_x) min_x = pos[v].x;
		if (pos[v].y < min_y) min_y = pos[v].y;
		if (pos[v].x > max_x) max_x = pos[v].x;
		if (pos[v].y > max_y) max_y = pos[v].y;
	}
	std::cout << "plot(c(),xlim=c(" << min_x << "," << max_x << "),ylim=c(" << min_y << "," << max_y << "))" << std::endl;
	//std::cout << "legend(4,4,0:" << (iterations-1) << ",fill=1:" << (iterations) << ")" << std::endl;
	for (NodeSharedPtr n: mnet->get_nodes()) {
		std::cout << "points(" << pos[n].x << "," << pos[n].y << ")" << std::endl;
	}
	for (EdgeSharedPtr e: mnet->get_edges()) {
		std::cout << "lines(c(" << pos[e->v1].x << "," << pos[e->v2].x << "),c(" << pos[e->v1].y << "," << pos[e->v2].y << "))" << std::endl;
	}
	//
	return pos;
}

}


