#ifndef PMM_H_
#define PMM_H_

#include "cutils.h"

namespace mlnet {

class pmm {

public:

	/*
		Use : glouvain g;
			 g.get_ml_community(MLNetworkSharedPtr);
	*/
	CommunitiesSharedPtr get_ml_community(MLNetworkSharedPtr mnet, unsigned int k, unsigned int ell, unsigned int maxKmeans);
	// http://spectralib.org/
	Eigen::MatrixXd modularitymaximization(Eigen::SparseMatrix<double> a, unsigned int ell);

};


}

#endif /* PMM_H_ */
