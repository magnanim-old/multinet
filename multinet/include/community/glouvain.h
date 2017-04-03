#ifndef GLOUVAIN_H_
#define GLOUVAIN_H_

#include "cutils.h"

namespace mlnet {

class glouvain {

public:

	/*
		Use : glouvain g;
			 g.get_ml_community(MLNetworkSharedPtr);
	*/
	CommunitiesSharedPtr get_ml_community(MLNetworkSharedPtr mnet, double gamma, double omega);
	Eigen::SparseMatrix<double> metanetwork(Eigen::SparseMatrix<double> B, std::vector<int> S2);

	Eigen::SparseMatrix<double> multicat(std::vector<Eigen::SparseMatrix<double>> a, double gamma, double omega);

	/* Map indexes of b to values of a: 
	https://stackoverflow.com/questions/5691218/matlab-mapping-values-to-index-of-other-array*/
	std::vector<int> mapV2I(std::vector<int> a, std::vector<int> b);

};


}

#endif
