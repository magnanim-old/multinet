#include <stdio.h>
#include <vector>
#include <iostream>
#include "layout.h"

namespace mlnet {

/** repulsive force */
double fr(double d, double k) {return k*k/d;}
/** attractive force, inter-layer */
double fain(double d, double k) {return d*d/k;}
/** attractive force, intra-layer */
double fainter(double d, double k) {return d*d/k;}

hash_map<NodeSharedPtr,coordinates> multiforce(const MLNetworkSharedPtr& mnet, double width, double length, const hash_map<LayerSharedPtr,double>& weight_in, const hash_map<LayerSharedPtr,double>& weight_inter, int iterations) {
	hash_map<NodeSharedPtr,coordinates> pos;
	hash_map<NodeSharedPtr,coordinates> disp;

	if (mnet->get_actors()->size()==0) return pos;

	double temp = std::sqrt(mnet->get_actors()->size());
	double start_temp = temp;
	double area = width*length;
	double k = std::sqrt(area/mnet->get_actors()->size());
	for (ActorSharedPtr a: *mnet->get_actors()) {
		double y = drand()*length-length/2;  // suggest to move these here
		double x = drand()*width-width/2; // suggest to move these here
		for (NodeSharedPtr n: *mnet->get_nodes(a)) {
			//double y = drand()*length-length/2;
			//double x = drand()*width-width/2;

			pos[n].y = x;
			pos[n].x = y;
			pos[n].z = mnet->get_layers()->get_index(n->layer);
		}
	}

	for (int i=0; i<iterations; i++) {
		// calculate repulsive forces
		for (LayerSharedPtr l: *mnet->get_layers()) {
			for (NodeSharedPtr v: *mnet->get_nodes(l)) {
				disp[v].x = 0;
				disp[v].y = 0;
				for (NodeSharedPtr u: *mnet->get_nodes(l)) {
					if (u == v) continue;
					coordinates Delta;
					Delta.x = pos[v].x - pos[u].x;
					Delta.y = pos[v].y - pos[u].y;
					//std::cout << "rep " << Delta.x << " " << Delta.y << std::endl;
					double DeltaNorm = std::sqrt(Delta.x*Delta.x+Delta.y*Delta.y);
					if (DeltaNorm==0) continue;
					disp[v].x = disp[v].x + Delta.x/DeltaNorm*fr(DeltaNorm,k)*weight_in.at(l);
					disp[v].y = disp[v].y + Delta.y/DeltaNorm*fr(DeltaNorm,k)*weight_in.at(l);
				}
			}
		}
		// calculate attractive forces inside each layer
		for (LayerSharedPtr l: *mnet->get_layers()) {
			for (EdgeSharedPtr e: *mnet->get_edges(l,l)) {
				NodeSharedPtr v = e->v1;
				NodeSharedPtr u = e->v2;
				coordinates Delta;
				Delta.x = pos[v].x - pos[u].x;
				Delta.y = pos[v].y - pos[u].y;
				//std::cout << "a-in " << Delta.x << " " << Delta.y << std::endl;
				double DeltaNorm = std::sqrt(Delta.x*Delta.x+Delta.y*Delta.y);
				if (DeltaNorm==0) continue;
				disp[v].x = disp[v].x - Delta.x/DeltaNorm*fain(DeltaNorm,k)*weight_in.at(l);
				disp[v].y = disp[v].y - Delta.y/DeltaNorm*fain(DeltaNorm,k)*weight_in.at(l);
				disp[u].x = disp[u].x + Delta.x/DeltaNorm*fain(DeltaNorm,k)*weight_in.at(l);
				disp[u].y = disp[u].y + Delta.y/DeltaNorm*fain(DeltaNorm,k)*weight_in.at(l);
			}
		}
		// calculate attractive forces across layers
		for (ActorSharedPtr a: *mnet->get_actors()) {
			for (NodeSharedPtr v: *mnet->get_nodes(a)) {
				for (NodeSharedPtr u: *mnet->get_nodes(a)) {
					if (v >= u) continue;
					coordinates Delta;
					Delta.x = pos[v].x - pos[u].x;
					Delta.y = pos[v].y - pos[u].y;
					//std::cout << "a-inter " << Delta.x << " " << Delta.y << std::endl;
					double DeltaNorm = std::sqrt(Delta.x*Delta.x+Delta.y*Delta.y);
					if (DeltaNorm==0) continue;
					disp[v].x = disp[v].x - Delta.x/DeltaNorm*fainter(DeltaNorm,k)*weight_inter.at(v->layer);
					disp[v].y = disp[v].y - Delta.y/DeltaNorm*fainter(DeltaNorm,k)*weight_inter.at(v->layer);
					disp[u].x = disp[u].x + Delta.x/DeltaNorm*fainter(DeltaNorm,k)*weight_inter.at(u->layer);
					disp[u].y = disp[u].y + Delta.y/DeltaNorm*fainter(DeltaNorm,k)*weight_inter.at(u->layer);
				}
			}
		}
		// assign new positions
		for (NodeSharedPtr v: *mnet->get_nodes()) {
			double dispNorm = std::sqrt(disp[v].x*disp[v].x+disp[v].y*disp[v].y);
			if (dispNorm==0) continue;
			pos[v].x = pos[v].x + (disp[v].x/dispNorm)*std::min(dispNorm,temp);
			pos[v].y = pos[v].y + (disp[v].y/dispNorm)*std::min(dispNorm,temp);
			//pos[v].x = std::min(width/2, std::max(-width/2, pos[v].x)); // suggest to remove
			//pos[v].y = std::min(length/2, std::max(-length/2, pos[v].y)); // suggest to remove
		}
		// reduce the temperature
		temp -= start_temp/iterations;
	}
	return pos;
}


double layout_eval_internal(const MLNetworkSharedPtr& mnet, const hash_map<NodeSharedPtr,coordinates>& pos, double width, double length) {
	double area = width*length;
	hash_map<NodeSharedPtr,coordinates> disp;
	double k = std::sqrt(area/mnet->get_actors()->size());
	// calculate repulsive forces
			for (LayerSharedPtr l: *mnet->get_layers()) {
				for (NodeSharedPtr v: *mnet->get_nodes(l)) {
					for (NodeSharedPtr u: *mnet->get_nodes(l)) {
						if (u == v) continue;
						coordinates Delta;
						Delta.x = pos.at(v).x - pos.at(u).x;
						Delta.y = pos.at(v).y - pos.at(u).y;
						//std::cout << "rep " << Delta.x << " " << Delta.y << std::endl;
						double DeltaNorm = std::sqrt(Delta.x*Delta.x+Delta.y*Delta.y);
						if (DeltaNorm==0) continue;
						disp[v].x = disp[v].x + Delta.x/DeltaNorm*fr(DeltaNorm,k);
						disp[v].y = disp[v].y + Delta.y/DeltaNorm*fr(DeltaNorm,k);
					}
				}
			}
			// calculate attractive forces inside each layer
			for (LayerSharedPtr l: *mnet->get_layers()) {
				for (EdgeSharedPtr e: *mnet->get_edges(l,l)) {
					NodeSharedPtr v = e->v1;
					NodeSharedPtr u = e->v2;
					coordinates Delta;
					Delta.x = pos.at(v).x - pos.at(u).x;
					Delta.y = pos.at(v).y - pos.at(u).y;
					//std::cout << "a-in " << Delta.x << " " << Delta.y << std::endl;
					double DeltaNorm = std::sqrt(Delta.x*Delta.x+Delta.y*Delta.y);
					if (DeltaNorm==0) continue;
					disp[v].x = disp[v].x - Delta.x/DeltaNorm*fain(DeltaNorm,k);
					disp[v].y = disp[v].y - Delta.y/DeltaNorm*fain(DeltaNorm,k);
					disp[u].x = disp[u].x + Delta.x/DeltaNorm*fain(DeltaNorm,k);
					disp[u].y = disp[u].y + Delta.y/DeltaNorm*fain(DeltaNorm,k);
				}
			}
			double avg_dist=0;
			for (auto d: disp) {
				avg_dist+=std::sqrt(d.second.x*d.second.x+d.second.y*d.second.y);
			}
			return avg_dist/mnet->get_nodes()->size();
}

double layout_eval_external(const MLNetworkSharedPtr& mnet, const hash_map<NodeSharedPtr,coordinates>& pos, double width, double length) {
	double area = width*length;
	hash_map<NodeSharedPtr,coordinates> disp;
		double k = std::sqrt(area/mnet->get_actors()->size());
		// calculate attractive forces across layers
			for (ActorSharedPtr a: *mnet->get_actors()) {
				for (NodeSharedPtr v: *mnet->get_nodes(a)) {
					for (NodeSharedPtr u: *mnet->get_nodes(a)) {
						if (v == u) continue;
						coordinates Delta;
						Delta.x = pos.at(v).x - pos.at(u).x;
						Delta.y = pos.at(v).y - pos.at(u).y;
						//std::cout << "a-inter " << Delta.x << " " << Delta.y << std::endl;
						double DeltaNorm = std::sqrt(Delta.x*Delta.x+Delta.y*Delta.y);
						if (DeltaNorm==0) continue;
						disp[v].x = disp[v].x - Delta.x/DeltaNorm*fainter(DeltaNorm,k);
						disp[v].y = disp[v].y - Delta.y/DeltaNorm*fainter(DeltaNorm,k);
						disp[u].x = disp[u].x + Delta.x/DeltaNorm*fainter(DeltaNorm,k);
						disp[u].y = disp[u].y + Delta.y/DeltaNorm*fainter(DeltaNorm,k);
					}
				}
			}
			double avg_dist=0;
			for (auto d: disp) {
				avg_dist+=std::sqrt(d.second.x*d.second.x+d.second.y*d.second.y);
			}
			return avg_dist/mnet->get_nodes()->size();
}
}


