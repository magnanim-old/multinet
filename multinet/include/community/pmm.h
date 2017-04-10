#ifndef PMM_H_
#define PMM_H_

#include "cutils.h"

namespace mlnet {

class pmm {

public:

	/*
		Use : pmm p;
		Data invariant:
			PMM is a multiplex community detector based on the paper Uncovering Groups via Heterogeneous Interaction Analysis by 
			Ltangasuedu, E and Wang, X. It uses http://spectralib.org/ to find top eigenvectors of the modularity matrix
	*/
	CommunitiesSharedPtr get_ml_community(MLNetworkSharedPtr mnet, unsigned int k, unsigned int ell, unsigned int maxKmeans);
	Eigen::MatrixXd modularitymaximization(Eigen::SparseMatrix<double> a, unsigned int ell);
};


}

#endif /* PMM_H_ */
