#ifndef PMM_H_
#define PMM_H_

#include "cutils.h"

namespace mlnet {

class pmm {

public:

	/*
		Use : p.fit(net, k, ell);
			PMM is a multiplex community detector based on the paper Uncovering Groups via Heterogeneous Interaction Analysis by
			Ltangasuedu, E and Wang, X (http://leitang.net/heterogeneous_network.html).
			It uses http://spectralib.org/ to find top eigenvectors of the modularity matrix

			+ mnet is the multilayer network to be analyzed
			+ k is the amount of clusters to return
			+ ell is number of structural features to extract from each dimension
	*/
	CommunityStructureSharedPtr fit(MLNetworkSharedPtr mnet, unsigned int k, unsigned int ell, double gamma);
	Eigen::MatrixXd modularitymaximization(Eigen::SparseMatrix<double> a, unsigned int ell, double gamma);
};


}

#endif /* PMM_H_ */
