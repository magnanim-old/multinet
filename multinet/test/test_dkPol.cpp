#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/multinet.h"
#include "../include/community/flattening.h"
#include "../include/community/acl.h"
#include "../include/community/glouvain.h"
#include "../include/community/lart.h"
#include "../include/community/pmm.h"
#include "../include/community/abacus.h"

using namespace mlnet;

void test_dkPol() {


	//Read the input Dataset
	MLNetworkSharedPtr mnet;
	double gamma ;
	//Discover Communities Using Principal modularity Maximization
	test_begin("ML-PMM");
		mnet = read_multilayer("dkpol.mpx","dkpol",',');
		pmm p;
		unsigned int k = 3;
		unsigned int ell = 2;
		gamma = 1.0;
		CommunityStructureSharedPtr pmm = p.fit(mnet, k, ell);
		std::cout << modularity(mnet, pmm, gamma) << std::endl;

	test_end("ML-PMM");

	//Discover Communities Using Redundancy Flattening
	test_begin("Flattening");
		mnet = read_multilayer("dkpol.mpx","dkpol",',');
	    CommunityStructureSharedPtr fl = flattenAndDetectComs(mnet,ZeroOne,LabelPropagation);
	    gamma = 1.0;
	    std::cout << modularity(mnet, fl, gamma) << std::endl;

	test_end("Flattening");

	//3) Discover Communities Using GLOUVAIN
	test_begin("ML-GLOUVAIN");
		mnet = read_multilayer("dkpol.mpx","dkpol",',');
		glouvain g;
		gamma = 1.0;
		double omega = 1.0;
		double limit = 3.0;
		std::string move = "move";
		CommunityStructureSharedPtr gl =  g.fit(mnet, move, gamma, omega,limit);
		std::cout << modularity(mnet, gl, gamma) << std::endl;

	test_end("ML-GLOUVAIN");

	//4) Discover Communities Using LART
	test_begin("ML-LART");

		mnet = read_multilayer("dkpol.mpx","dkpol",',');
		lart l;
		uint32_t t = 9;
		double eps = 1;
		gamma = 1;
		CommunityStructureSharedPtr la = l.fit(mnet, t, eps, gamma);
		std::cout << modularity(mnet, la, gamma) << std::endl;
		//CommunityStructureSharedPtr truth = read_truth2(mnet);
		//std::cout << normalized_mutual_information(la, truth, mnet->get_nodes()->size()) << std::endl;

	test_end("ML-LART");

	//5) Discover Communities Using clique percolation
	test_begin("MLCPM");

			//double sizeK = 3; //min num of actors in a clique
			//double sizeM = 4;//min num if layers in a clique
			//CommunityStructureSharedPtr la = mlcpm(mnet,sizeK,sizeM);
			//std::cout << modularity(mnet, la, gamma) << std::endl;

			//CommunityStructureSharedPtr truth = read_truth2(mnet);
			//std::cout << normalized_mutual_information(la, truth, mnet->get_nodes()->size()) << std::endl;

	test_end("MLCPM");

	//6) Discover Communities Using ABACUS
	test_begin("ABACUS");

				mnet = read_multilayer("dkpol.mpx","dkpol",',');
				double sizeK = 3; //min num of actors
				double sizeM = 3;//min num if layers in
				ActorCommunityStructureSharedPtr aba = abacus(mnet,sizeK,sizeM);
				std::cout << modularity(mnet, to_node_communities(aba,mnet), gamma) << std::endl;

				//CommunityStructureSharedPtr truth = read_truth2(mnet);
				//std::cout << normalized_mutual_information(aba, truth, mnet->get_nodes()->size()) << std::endl;

	test_end("ABACUS");
}


