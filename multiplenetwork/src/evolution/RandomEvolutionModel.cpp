#include <set>
#include <cstdlib>
#include <iostream>
#include "evolution.h"

using namespace std;

RandomEvolutionModel::RandomEvolutionModel(int m0, double p) {
	RandomEvolutionModel::m0 = m0;
	RandomEvolutionModel::p = p;
}


RandomEvolutionModel::~RandomEvolutionModel() {
	//
}


void RandomEvolutionModel::evolution_step(MultilayerNetwork& mnet, network_id net) {
	// This is a static model that does not evolve!
}


void RandomEvolutionModel::init_step(MultilayerNetwork& mnet, network_id net) {
	/*
	if (mnet.getNumGlobalVertexes()<m0) {
		// TODO
		m0 = mnet.getNumVertexes();
	}

	set<unsigned long> res = r.getKRandom(mnet.getNumGlobalVertexes(), m0);
	for (set<unsigned long>::iterator it=res.begin(); it!=res.end(); ++it) {
		mnet.addLocalVertex(*it,net);
	}

	// Now for each pair of vertexes create a link with probability p
	for (set<unsigned long>::iterator it1=res.begin(); it1!=res.end(); ++it1) {
		for (set<unsigned long>::iterator it2=res.begin(); it2!=res.end(); ++it2) {
			if (r.test(p)) {
				// create a link
				// If the network is undirected, do not create it twice (which would raise an exception)
				if (mnet.isDirected(net) || *it1<*it2)
					mnet.addGlobalEdge(*it1,*it2,net);
			}
		}
	}*/
}





